#ifndef AGPU_IMMEDIATE_RENDERER_HPP
#define AGPU_IMMEDIATE_RENDERER_HPP

#include "state_tracker_cache.hpp"
#include "vector_math.hpp"
#include <vector>
#include <functional>

namespace AgpuCommon
{

class ImmediateShaderLibrary
{
public:
    agpu::shader_ref flatColorVertex;
    agpu::shader_ref flatLightedColorVertex;
    agpu::shader_ref flatColorFragment;

    agpu::shader_ref flatTexturedVertex;
    agpu::shader_ref flatLightedTexturedVertex;
    agpu::shader_ref flatTexturedFragment;

    agpu::shader_ref smoothLightedColorVertex;
    agpu::shader_ref smoothColorVertex;
    agpu::shader_ref smoothColorFragment;

    agpu::shader_ref smoothLightedTexturedVertex;
    agpu::shader_ref smoothTexturedVertex;
    agpu::shader_ref smoothTexturedFragment;
};

class ImmediateRendererSamplerState
{
public:
    agpu::sampler_ref sampler;
    agpu::shader_resource_binding_ref binding;
};

class ImmediateSharedRenderingStates
{
public:
    ImmediateRendererSamplerState linearSampler;
};

/**
 * Immediate renderer vertex.
 */
struct ImmediateRendererVertex
{
    Vector2F texcoord;
    Vector3F normal;
    Vector3F position;
    Vector4F color;
};

struct ImmediateRenderingState
{
    agpu_primitive_topology activePrimitiveTopology;
    bool flatShading;
    bool lightingEnabled;
    bool texturingEnabled;

    agpu::texture_ref activeTexture;
};

struct ImmediatePushConstants
{
    uint32_t projectionMatrixIndex;
    uint32_t modelViewMatrixIndex;
    uint32_t textureMatrixIndex;
    uint32_t lightingStateIndex;
    uint32_t materialStateIndex;
};

struct LightState
{
    LightState();

    size_t hash() const;
    bool operator==(const LightState &other) const;

    Vector4F ambientColor;
    Vector4F diffuseColor;
    Vector4F specularColor;

    Vector4F position;
    Vector3F spotDirection;
    float spotCosCutoff;

    float spotExponent;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

struct LightingState
{
    static constexpr size_t MaxLightCount = 8;

    LightingState();

    size_t hash() const;
    bool operator==(const LightingState &other) const;

    Vector4F ambientLighting;

    uint32_t enabledLightMask;
    uint32_t padding[3];
    std::array<LightState, MaxLightCount> lights;
};

struct MaterialState
{
    MaterialState();

    size_t hash() const;
    bool operator==(const MaterialState &other) const;

    Vector4F emission;
    Vector4F ambient;
    Vector4F diffuse;
    Vector4F specular;

    float shininess;
    uint32_t padding[3];
};

}

namespace std
{
template<>
struct hash<AgpuCommon::LightState>
{
    size_t operator()(const AgpuCommon::LightState &ref) const
    {
        return ref.hash();
    }
};

template<>
struct hash<AgpuCommon::LightingState>
{
    size_t operator()(const AgpuCommon::LightingState &ref) const
    {
        return ref.hash();
    }
};

template<>
struct hash<AgpuCommon::MaterialState>
{
    size_t operator()(const AgpuCommon::MaterialState &ref) const
    {
        return ref.hash();
    }
};

}

namespace AgpuCommon
{
/**
 * I am an immediate renderer that emulates a classic OpenGL style
 * glBegin()/glEnd() rendering interface.
 */
class ImmediateRenderer : public agpu::immediate_renderer
{
public:
    typedef std::vector<Matrix4F> MatrixStack;

    static constexpr size_t GpuBufferDataThreshold = 1024*256;

    ImmediateRenderer(const agpu::state_tracker_cache_ref &stateTrackerCache);
    ~ImmediateRenderer();

    static agpu::immediate_renderer_ref create(const agpu::state_tracker_cache_ref &cache);

    virtual agpu_error beginRendering(const agpu::state_tracker_ref & state_tracker) override;
	virtual agpu_error endRendering() override;

    // Pipeline state. Delegated to the state tracker.
    virtual agpu_error setBlendState(agpu_int renderTargetMask, agpu_bool enabled) override;
    virtual agpu_error setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation) override;
    virtual agpu_error setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled) override;
    virtual agpu_error setFrontFace(agpu_face_winding winding) override;
    virtual agpu_error setCullMode(agpu_cull_mode mode) override;
    virtual agpu_error setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor) override;
    virtual agpu_error setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function) override;
    virtual agpu_error setPolygonMode(agpu_polygon_mode mode) override;
    virtual agpu_error setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask) override;
    virtual agpu_error setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;
    virtual agpu_error setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;

    // Rendering commands
    virtual agpu_error setViewport(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
    virtual agpu_error setScissor(agpu_int x, agpu_int y, agpu_int w, agpu_int h) override;
    virtual agpu_error setStencilReference(agpu_uint reference) override;

    // Matrix stack
    virtual agpu_error projectionMatrixMode() override;
	virtual agpu_error modelViewMatrixMode() override;
	virtual agpu_error textureMatrixMode() override;
	virtual agpu_error loadIdentity() override;
    virtual agpu_error pushMatrix() override;
	virtual agpu_error popMatrix() override;
	virtual agpu_error loadMatrix(agpu_float* elements) override;
	virtual agpu_error loadTransposeMatrix(agpu_float* elements) override;
	virtual agpu_error multiplyMatrix(agpu_float* elements) override;
	virtual agpu_error multiplyTransposeMatrix(agpu_float* elements) override;

	virtual agpu_error ortho(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far) override;
    virtual agpu_error frustum(agpu_float left, agpu_float right, agpu_float bottom, agpu_float top, agpu_float near, agpu_float far) override;
	virtual agpu_error perspective(agpu_float fovy, agpu_float aspect, agpu_float near, agpu_float far) override;
    virtual agpu_error rotate(agpu_float angle, agpu_float x, agpu_float y, agpu_float z) override;
	virtual agpu_error translate(agpu_float x, agpu_float y, agpu_float z) override;
	virtual agpu_error scale(agpu_float x, agpu_float y, agpu_float z) override;

    // Fixed function pipeline states
    virtual agpu_error setFlatShading(agpu_bool enabled) override;
    virtual agpu_error setLightingEnabled(agpu_bool enabled) override;
    virtual agpu_error clearLights() override;
    virtual agpu_error setAmbientLighting(agpu_float r, agpu_float g, agpu_float b, agpu_float a) override;
    virtual agpu_error setLight(agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state) override;
    virtual agpu_error setTexturingEnabled(agpu_bool enabled) override;
    virtual agpu_error bindTexture(const agpu::texture_ref &texture) override;

    // Geometry
	virtual agpu_error beginPrimitives(agpu_primitive_topology type) override;
	virtual agpu_error endPrimitives() override;
	virtual agpu_error color(agpu_float r, agpu_float g, agpu_float b, agpu_float a) override;
	virtual agpu_error texcoord(agpu_float x, agpu_float y) override;
	virtual agpu_error normal(agpu_float x, agpu_float y, agpu_float z) override;
	virtual agpu_error vertex(agpu_float x, agpu_float y, agpu_float z) override;

    virtual agpu_error beginMeshWithVertices(agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer vertices) override;
	virtual agpu_error setCurrentMeshColors(agpu_size stride, agpu_size elementCount, agpu_pointer colors) override;
	virtual agpu_error setCurrentMeshNormals(agpu_size stride, agpu_size elementCount, agpu_pointer normals) override;
	virtual agpu_error setCurrentMeshTexCoords(agpu_size stride, agpu_size elementCount, agpu_pointer texcoords) override;
	virtual agpu_error drawElementsWithIndices(agpu_primitive_topology mode, agpu_pointer indices, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) override;
	virtual agpu_error endMesh() override;


private:
    typedef std::function<void ()> PendingRenderingCommand;

    void applyMatrix(const Matrix4F &matrix);
    void invalidateMatrix();
    agpu_error validateMatrices();
    agpu_error validateLightingState();
    agpu_error validateMaterialState();
    agpu_error validateRenderingStates();

    agpu_error flushRenderingState(const ImmediateRenderingState &state);
    agpu_error flushRenderingData();
    void uploadPushConstants();

    agpu::shader_resource_binding_ref getValidTextureBindingFor(const agpu::texture_ref &texture);

    template<typename FT>
    agpu_error delegateToStateTracker(const FT &f)
    {
        if(!currentStateTracker)
            return AGPU_INVALID_OPERATION;
        pendingRenderingCommands.push_back(f);
        return AGPU_OK;
    }

    // Common state
    agpu::device_ref device;
    agpu::state_tracker_cache_ref stateTrackerCache;
    agpu::state_tracker_ref currentStateTracker;

    agpu::shader_signature_ref immediateShaderSignature;
    ImmediateShaderLibrary *immediateShaderLibrary;
    ImmediateSharedRenderingStates *immediateSharedRenderingStates;
    agpu::vertex_layout_ref immediateVertexLayout;
    agpu::vertex_binding_ref vertexBinding;
    agpu::shader_resource_binding_ref uniformResourceBindings;

    // The rendering state.
    ImmediateRenderingState currentRenderingState;
    ImmediateRendererVertex currentVertex;
    size_t lastDrawnVertexIndex;

    std::vector<PendingRenderingCommand> pendingRenderingCommands;

    ImmediatePushConstants currentPushConstants;

    // Vertices
    agpu::buffer_ref vertexBuffer;
    size_t vertexBufferCapacity;
    std::vector<ImmediateRendererVertex> vertices;

    // Indices
    agpu::buffer_ref indexBuffer;
    size_t indexBufferCapacity;
    std::vector<uint32_t> indices;

    // Immediate mesh
    bool renderingImmediateMesh;
    size_t currentImmediateMeshBaseVertex;
    size_t currentImmediateMeshVertexCount;

    // Matrices
    MatrixStack projectionMatrixStack;
    bool projectionMatrixStackDirtyFlag;

    MatrixStack modelViewMatrixStack;
    bool modelViewMatrixStackDirtyFlag;

    MatrixStack textureMatrixStack;
    bool textureMatrixStackDirtyFlag;

    MatrixStack *activeMatrixStack;
    bool *activeMatrixStackDirtyFlag;

    agpu::buffer_ref matrixBuffer;
    size_t matrixBufferCapacity;
    std::vector<Matrix4F> matrixBufferData;

    // Texture bindings
    std::vector<agpu::shader_resource_binding_ref> allocatedTextureBindings;
    std::unordered_map<agpu::texture_ref, agpu::shader_resource_binding_ref> usedTextureBindingMap;
    size_t usedTextureBindingCount;

    // Lighting state
    LightingState currentLightingState;
    bool currentLightingStateDirty;
    size_t lightingStateBufferCapacity;
    std::vector<LightingState> lightingStateBufferData;
    agpu::buffer_ref lightingStateBuffer;

    // Material state.
    MaterialState currentMaterialState;
    bool currentMaterialStateDirty;
    size_t materialStateBufferCapacity;
    std::vector<MaterialState> materialStateBufferData;
    agpu::buffer_ref materialStateBuffer;
};

} // End of namespace AgpuCommon

#endif //AGPU_IMMEDIATE_RENDERER_HPP
