#ifndef AGPU_IMMEDIATE_RENDERER_HPP
#define AGPU_IMMEDIATE_RENDERER_HPP

#include "state_tracker_cache.hpp"
#include "vector_math.hpp"
#include "utility.hpp"
#include <assert.h>
#include <vector>
#include <functional>
#include <string.h>

namespace AgpuCommon
{

struct ImmediateShaderCompilationParameters
{
    ImmediateShaderCompilationParameters()
        : flatShading(false),
        texturingEnabled(false),
        tangentSpaceEnabled(false),
        skinningEnabled(false),
        lightingEnabled(false),
        lightingModel(AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_PER_VERTEX)
    {}

    bool operator==(const ImmediateShaderCompilationParameters &other) const;
    size_t hash() const;

    std::string shaderOptionsString(agpu_shader_type type) const;

    bool flatShading;
    bool texturingEnabled;
    bool tangentSpaceEnabled;
    bool skinningEnabled;
    bool lightingEnabled;
    agpu_immediate_renderer_lighting_model lightingModel;
};

struct ImmediateTextureBindingSet
{
    agpu::texture_ref albedoTexture;
    agpu::texture_ref emissionTexture;
    agpu::texture_ref normalTexture;
    agpu::texture_ref occlusionTexture;
    agpu::texture_ref roughnessMetallicTexture;

    size_t hash() const;
    bool operator==(const ImmediateTextureBindingSet& other) const;
    bool operator!=(const ImmediateTextureBindingSet& other) const;
};

struct ImmediateRendererSamplerStateDescription
{
    agpu_filter filter;
    float maxAnisotropy;
    agpu_texture_address_mode addressU;
    agpu_texture_address_mode addressV;
    agpu_texture_address_mode addressW;

    bool operator==(const ImmediateRendererSamplerStateDescription &other) const;
    bool operator!=(const ImmediateRendererSamplerStateDescription &other) const;
    size_t hash() const;
};

}

namespace std
{
template<>
struct hash<AgpuCommon::ImmediateShaderCompilationParameters>
{
    size_t operator()(const AgpuCommon::ImmediateShaderCompilationParameters &ref) const
    {
        return ref.hash();
    }
};

template<>
struct hash<AgpuCommon::ImmediateTextureBindingSet>
{
    size_t operator()(const AgpuCommon::ImmediateTextureBindingSet& ref) const
    {
        return ref.hash();
    }
};

template<>
struct hash<AgpuCommon::ImmediateRendererSamplerStateDescription>
{
    size_t operator()(const AgpuCommon::ImmediateRendererSamplerStateDescription &ref) const
    {
        return ref.hash();
    }
};

}

namespace AgpuCommon
{
class ImmediateShaderLibrary
{
public:

    agpu::shader_ref getOrCreateWithCompilationParameters(const agpu::device_ref &device, const ImmediateShaderCompilationParameters &params, agpu_shader_type type);

private:
    std::mutex shaderCompilationMutex;
    std::unordered_map<ImmediateShaderCompilationParameters, agpu::shader_ref> vertexShaderCache;
    std::unordered_map<ImmediateShaderCompilationParameters, agpu::shader_ref> fragmentShaderCache;
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
    agpu::shader_signature_ref shaderSignature;
    agpu::device_ref device;

    agpu::shader_resource_binding_ref defaultSampler;
    agpu::texture_ref defaultAlbedoTexture;
    agpu::texture_ref defaultEmissionTexture;
    agpu::texture_ref defaultNormalTexture;
    agpu::texture_ref defaultOcclusionTexture;
    agpu::texture_ref defaultRoughnessMetallicTexture;

    agpu::shader_resource_binding_ref &getSamplerStateBindingFor(const ImmediateRendererSamplerStateDescription &description);

private:
    std::mutex samplerStatesMutex;
    std::unordered_map<ImmediateRendererSamplerStateDescription, ImmediateRendererSamplerState> samplerStates;
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
    ImmediateRenderingState()
        : activePrimitiveTopology(AGPU_POINTS),
          flatShading(false),
          lightingEnabled(false),
          lightingModel(AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_PER_VERTEX),
          texturingEnabled(false),
          tangentSpaceEnabled(false),
          skinningEnabled(false) {}

    agpu_primitive_topology activePrimitiveTopology;
    bool flatShading;
    bool lightingEnabled;
    agpu_immediate_renderer_lighting_model lightingModel;
    bool texturingEnabled;
    bool tangentSpaceEnabled;
    bool skinningEnabled;

    agpu::shader_resource_binding_ref samplingStateBinding;
    agpu::shader_resource_binding_ref lightingStateBinding;
    agpu::shader_resource_binding_ref extraRenderingStateBinding;
    agpu::shader_resource_binding_ref materialStateBinding;
    agpu::shader_resource_binding_ref transformationStateBinding;
    agpu::shader_resource_binding_ref skinningStateBinding;
    ImmediateTextureBindingSet textureBindingSet;
};

struct TransformationState
{
    TransformationState ()
        :
        projectionMatrix(Matrix4F::identity()),
        modelViewMatrix(Matrix4F::identity()),
        inverseModelViewMatrix(Matrix4F::identity()),
        textureMatrix(Matrix4F::identity())
    {};
    TransformationState(const TransformationState &other)
        :
        projectionMatrix(other.projectionMatrix),
        modelViewMatrix(other.modelViewMatrix),
        inverseModelViewMatrix(other.inverseModelViewMatrix),
        textureMatrix(other.textureMatrix)
    {};

    bool operator==(const TransformationState &other) const;
    size_t hash() const;

    // This normally does not change.
    Matrix4F projectionMatrix;

    // We need the inverse for transforming normals.
    Matrix4F modelViewMatrix;
    Matrix4F inverseModelViewMatrix;

    // The texture matrix.
    Matrix4F textureMatrix;
};

static_assert(sizeof(TransformationState) % 256 == 0, "TransformationState requires an aligned size of 256 bytes");

struct SkinningState
{
    static constexpr int MaxNumberOfBones = 128;

    SkinningState ()
    {
        for(int i = 0; i < 128; ++i)
            boneMatrices[i] = Matrix4F::identity();
    }

    SkinningState(const SkinningState &other)
    {
        for(int i = 0; i < 128; ++i)
            boneMatrices[i] = other.boneMatrices[i];
    }

    bool operator==(const SkinningState &other) const;
    bool operator!=(const SkinningState &other) const;
    size_t hash() const;

    Matrix4F boneMatrices[128];
};

static_assert(sizeof(SkinningState) % 256 == 0, "SkinningState requires an aligned size of 256 bytes");

struct ClassicLightState
{
    ClassicLightState();

    size_t hash() const;
    bool operator==(const ClassicLightState &other) const;

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
static_assert(sizeof(ClassicLightState) == 96, "For manual alignment");

struct PBRLightState
{
    PBRLightState();

    size_t hash() const;
    bool operator==(const PBRLightState &other) const;

    Vector4F ambient;
    Vector4F intensity;

    Vector4F position;
    Vector3F spotDirection;
    float spotCosCutoff;

    float spotExponent;
    float spotInnerCosCutoff;
    float radius;
    float padding[5];
};
static_assert(sizeof(PBRLightState) == sizeof(ClassicLightState), "For manual alignment");

struct LightState
{
    LightState();

    size_t hash() const;
    bool operator==(const LightState &other) const;

    union
    {
        ClassicLightState classic;
        PBRLightState pbr;
    };
};

static_assert(sizeof(LightState) == 96, "For manual alignment");

struct LightingState
{
    static constexpr size_t MaxLightCount = 8;

    LightingState();

    size_t hash() const;
    bool operator==(const LightingState &other) const;
    bool operator!=(const LightingState &other) const;

    Vector4F ambientLighting;

    uint32_t enabledLightMask;
    uint32_t padding[3];
    std::array<LightState, MaxLightCount> lights;
    uint8_t extraPadding[224];
};

static_assert(sizeof(LightingState) % 256 == 0, "LightingState requires an aligned size of 256 bytes");

enum MaterialStateType : uint32_t
{
    Classic = 0,
    MetallicRoughness,
    FlatColor
};

struct ClassicMaterialState
{
    static ClassicMaterialState defaultMaterial();

    size_t hash() const;
    bool operator==(const ClassicMaterialState &other) const;
    bool operator!=(const ClassicMaterialState &other) const;

    MaterialStateType type;
    float shininess;
    float alphaCutoff;
    uint32_t padding;

    Vector4F emission;
    Vector4F ambient;
    Vector4F diffuse;
    Vector4F specular;
};

struct MetallicRoughnessMaterialState
{
    static MetallicRoughnessMaterialState defaultMaterial();

    size_t hash() const;
    bool operator==(const MetallicRoughnessMaterialState &other) const;
    bool operator!=(const MetallicRoughnessMaterialState &other) const;

    MaterialStateType type;
    float roughnessFactor;
    float metallicFactor;
    float occlusionFactor;

    float alphaCutoff;
    uint32_t padding[3];

    Vector4F emission;
    Vector4F baseColor;
};

struct FlatColorMaterialState
{
    static FlatColorMaterialState defaultMaterial();

    size_t hash() const;
    bool operator==(const FlatColorMaterialState &other) const;
    bool operator!=(const FlatColorMaterialState &other) const;

    MaterialStateType type;
    float alphaCutoff;
    uint32_t padding[2];

    Vector4F color;
};

struct MaterialState
{
    MaterialState();

    size_t hash() const;
    bool operator==(const MaterialState &other) const;
    bool operator!=(const MaterialState &other) const;

    union
    {
        MaterialStateType type;

        ClassicMaterialState classic;
        MetallicRoughnessMaterialState metallicRoughness;
        FlatColorMaterialState flat;
    };

    uint8_t extraPadding[176];
};

static_assert(sizeof(MaterialState) % 256 == 0, "MaterialState requires an aligned size of 256 bytes");

struct ExtraRenderingState
{
    ExtraRenderingState();

    size_t hash() const;
    bool operator==(const ExtraRenderingState &other) const;
    bool operator!=(const ExtraRenderingState &other) const;

    Vector4F userClipPlane;

    uint32_t fogMode;
    float fogStartDistance;
    float fogEndDistance;
    float fogDensity;

    Vector4F fogColor;

    uint8_t extraPadding[208];
};

static_assert(sizeof(ExtraRenderingState) % 256 == 0, "ExtraRenderingState requires an aligned size of 256 bytes");
}

namespace std
{

template<>
struct hash<AgpuCommon::TransformationState>
{
    size_t operator()(const AgpuCommon::TransformationState &ref) const
    {
        return ref.hash();
    }
};

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

template<>
struct hash<AgpuCommon::ExtraRenderingState>
{
    size_t operator()(const AgpuCommon::ExtraRenderingState &ref) const
    {
        return ref.hash();
    }
};

template<>
struct hash<AgpuCommon::SkinningState>
{
    size_t operator()(const AgpuCommon::SkinningState &ref) const
    {
        return ref.hash();
    }
};
}

namespace AgpuCommon
{

template<typename ST, agpu_uint DS>
class ImmediateStateBuffer
{
public:
    static constexpr size_t GpuBufferDataThreshold = 1024*256;
    static constexpr agpu_uint DescriptorSetIndex = DS;

    typedef ST StateType;
    typedef ImmediateStateBuffer<StateType, DescriptorSetIndex> SelfType;

    static_assert(sizeof(StateType) % 256 == 0, "Uniform constant structures must be aligned to 256 bytes");

	ImmediateStateBuffer(const agpu::shader_signature_ref &cshaderSignature)
		: dirtyFlag(false), bufferCapacity(0), shaderSignature(cshaderSignature)
    {

    }
    ~ImmediateStateBuffer()
    {
    }

    void reset()
    {
        currentState = StateType();
        dirtyFlag = true;
        currentStateIndex = 0;
        bufferData.clear();
        stateCache.clear();
    }

    void makeDirty()
    {
        dirtyFlag = true;
    }

    bool isDirty() const
    {
        return dirtyFlag;
    }

    const agpu::shader_resource_binding_ref &validateCurrentState()
    {
        if(isDirty() || bufferData.empty())
        {
            auto it = stateCache.find(currentState);
            if(it != stateCache.end())
            {
                currentStateIndex = it->second;
            }
            else
            {
                bufferData.push_back(currentState);
                ensureValidResourceBinding();
                currentStateIndex = bufferData.size() - 1;
                stateCache.insert(std::make_pair(currentState, currentStateIndex));
            }

            dirtyFlag = false;
        }

        return resourceBindings[currentStateIndex];
    }

    void ensureValidResourceBinding()
    {
        auto requestedIndex = bufferData.size() - 1;
        if(requestedIndex < resourceBindings.size())
            return;
        assert(requestedIndex == resourceBindings.size());

        auto newBinding = agpu::shader_resource_binding_ref(shaderSignature->createShaderResourceBinding(DescriptorSetIndex));
        if(!newBinding)
        {
            fprintf(stderr, "Fatal error: failed to allocate a required shader resource binding\n");
            abort();
            return;
        }

        // Bind the descriptor to the buffer, only if it has the required capacity.
        if(buffer && requestedIndex < bufferCapacity)
        {
            newBinding->bindUniformBufferRange(0, buffer, sizeof(StateType)*requestedIndex, sizeof(StateType));
        }

        resourceBindings.push_back(newBinding);
    }

    void setState(const StateType &newState)
    {
        if(currentState != newState)
        {
            currentState = newState;
            dirtyFlag = true;
        }
    }

    agpu_error uploadData(const agpu::device_ref &device)
    {
        if(!buffer || bufferCapacity < bufferData.size())
        {
            bufferCapacity = nextPowerOfTwo(bufferData.size());
            if(bufferCapacity < 32)
                bufferCapacity = 32;

            auto requiredSize = bufferCapacity*sizeof(StateType);
            agpu_buffer_description bufferDescription = {};
            bufferDescription.size = requiredSize;
            bufferDescription.heap_type = requiredSize >= GpuBufferDataThreshold
                ? AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL : AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE;
            bufferDescription.usage_modes = bufferDescription.main_usage_mode = AGPU_UNIFORM_BUFFER;
            bufferDescription.mapping_flags = AGPU_MAP_DYNAMIC_STORAGE_BIT;
            bufferDescription.stride = sizeof(StateType);

            buffer = agpu::buffer_ref(device->createBuffer(&bufferDescription, nullptr));
            if(!buffer)
                return AGPU_OUT_OF_MEMORY;

            size_t bindingSize = sizeof(StateType);
            size_t bindingOffset = 0;
            for(size_t i = 0; i < resourceBindings.size(); ++i)
            {
                resourceBindings[i]->bindUniformBufferRange(0, buffer, bindingOffset, bindingSize);
                bindingOffset += bindingSize;
            }
        }

        if(!bufferData.empty())
        {
            auto error = buffer->uploadBufferData(0, bufferData.size()*sizeof(StateType), &bufferData[0]);
            if(error)
                return error;
        }

        return AGPU_OK;
    }

    StateType currentState;
    bool dirtyFlag;

    size_t bufferCapacity;
    size_t currentStateIndex;
    std::vector<StateType> bufferData;
    std::unordered_map<StateType, size_t> stateCache;
    std::vector<agpu::shader_resource_binding_ref> resourceBindings;
    const agpu::shader_signature_ref &shaderSignature;
    agpu::buffer_ref buffer;
};

/**
 * I am an immediate renderer that emulates a classic OpenGL style
 * glBegin()/glEnd() rendering interface.
 */
class ImmediateRenderer : public agpu::immediate_renderer
{
public:
    typedef std::vector<Matrix4F> MatrixStack;

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
    virtual agpu_error setLightingModel(agpu_immediate_renderer_lighting_model model) override;
    virtual agpu_error clearLights() override;
    virtual agpu_error setAmbientLighting(agpu_float r, agpu_float g, agpu_float b, agpu_float a) override;
    virtual agpu_error setLight(agpu_uint index, agpu_bool enabled, agpu_immediate_renderer_light* state) override;
    virtual agpu_error setMaterial(agpu_immediate_renderer_material* state) override;
    virtual agpu_error setSkinningEnabled(agpu_bool enabled) override;
	virtual agpu_error setSkinBones(agpu_uint count, agpu_float* matrices, agpu_bool transpose) override;
    virtual agpu_error setTexturingEnabled(agpu_bool enabled) override;
    virtual agpu_error setSamplingMode(agpu_filter filter, agpu_float maxAnisotropy, agpu_texture_address_mode addressU, agpu_texture_address_mode addressV, agpu_texture_address_mode addressW) override;
	virtual agpu_error setTangentSpaceEnabled(agpu_bool enabled) override;
    virtual agpu_error bindTexture(const agpu::texture_ref &texture) override;
	virtual agpu_error bindTextureIn(const agpu::texture_ref & texture, agpu_immediate_renderer_texture_binding binding) override;
    virtual agpu_error setClipPlane(agpu_uint index, agpu_bool enabled, agpu_float p1, agpu_float p2, agpu_float p3, agpu_float p4) override;
    virtual agpu_error setFogMode(agpu_immediate_renderer_fog_mode mode) override;
	virtual agpu_error setFogColor(agpu_float r, agpu_float g, agpu_float b, agpu_float a) override;
	virtual agpu_error setFogDistances(agpu_float start, agpu_float end) override;
	virtual agpu_error setFogDensity(agpu_float density) override;

    // Geometry
	virtual agpu_error beginPrimitives(agpu_primitive_topology type) override;
	virtual agpu_error endPrimitives() override;
	virtual agpu_error color(agpu_float r, agpu_float g, agpu_float b, agpu_float a) override;
	virtual agpu_error texcoord(agpu_float x, agpu_float y) override;
	virtual agpu_error normal(agpu_float x, agpu_float y, agpu_float z) override;
	virtual agpu_error vertex(agpu_float x, agpu_float y, agpu_float z) override;

    virtual agpu_error beginMeshWithVertices(agpu_size vertexCount, agpu_size stride, agpu_size elementCount, agpu_pointer vertices) override;
    virtual agpu_error beginMeshWithVertexBinding(const agpu::vertex_layout_ref & layout, const agpu::vertex_binding_ref & vertices) override;
    virtual agpu_error useIndexBuffer(const agpu::buffer_ref & index_buffer) override;
    virtual agpu_error useIndexBufferAt(const agpu::buffer_ref & index_buffer, agpu_size offset, agpu_size index_size) override;
	virtual agpu_error setCurrentMeshColors(agpu_size stride, agpu_size elementCount, agpu_pointer colors) override;
	virtual agpu_error setCurrentMeshNormals(agpu_size stride, agpu_size elementCount, agpu_pointer normals) override;
	virtual agpu_error setCurrentMeshTexCoords(agpu_size stride, agpu_size elementCount, agpu_pointer texcoords) override;
    virtual agpu_error setPrimitiveType(agpu_primitive_topology type) override;
    virtual agpu_error drawArrays(agpu_uint vertex_count, agpu_uint instance_count, agpu_uint first_vertex, agpu_uint base_instance) override;
	virtual agpu_error drawElements(agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) override;
	virtual agpu_error drawElementsWithIndices(agpu_primitive_topology mode, agpu_pointer indices, agpu_uint index_count, agpu_uint instance_count, agpu_uint first_index, agpu_int base_vertex, agpu_uint base_instance) override;
	virtual agpu_error endMesh() override;


private:
    typedef std::function<void ()> PendingRenderingCommand;

    void applyMatrix(const Matrix4F &matrix);
    void invalidateMatrix();
    agpu_error validateTransformationState();
    agpu_error validateSkinningState();
    agpu_error validateLightingState();
    agpu_error validateMaterialState();
    agpu_error validateRenderingStates();
    agpu_error validateExtraRenderingState();

    agpu_error flushShadersForRenderingState(const ImmediateRenderingState &state);
    agpu_error flushRenderingState(const ImmediateRenderingState &state);
    agpu_error flushImmediateVertexRenderingState();
    agpu_error flushRenderingData();

    agpu::shader_resource_binding_ref getValidTextureBindingFor(const ImmediateTextureBindingSet &bindingSet);

    template<typename FT>
    agpu_error delegateToStateTracker(const FT &f)
    {
        if(!currentStateTracker)
            return AGPU_INVALID_OPERATION;
        pendingRenderingCommands.push_back(f);
        return AGPU_OK;
    }

    bool hasMetallicRoughnessLighting() const
    {
        return currentRenderingState.lightingModel == AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_METALLIC_ROUGHNESS;
    }

    bool hasFlatColorLightingMode() const
    {
        return currentRenderingState.lightingModel == AGPU_IMMEDIATE_RENDERER_LIGHTING_MODEL_FLAT_COLOR;
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

    // The rendering state.
    ImmediateRenderingState currentRenderingState;
    ImmediateRendererVertex currentVertex;
    size_t lastDrawnVertexIndex;

    ImmediateRenderingState lastFlushedRenderingState;
    bool haveFlushedRenderingState;
    std::vector<PendingRenderingCommand> pendingRenderingCommands;

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
    bool haveExplicitVertexBinding;
    bool haveExplicitIndexBuffer;
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

    // Lighting state
    ImmediateStateBuffer<LightingState, 1> lightingStateBuffer;

    // Extra rendering state
    ImmediateStateBuffer<ExtraRenderingState, 2> extraRenderingStateBuffer;

    // Material state.
    ImmediateStateBuffer<MaterialState, 3> materialStateBuffer;

    // Transformation state buffer
    ImmediateStateBuffer<TransformationState, 4> transformationStateBuffer;

    // Skinning state buffer
    ImmediateStateBuffer<SkinningState, 5> skinningStateBuffer;

    // Texture bindings
    std::vector<agpu::shader_resource_binding_ref> allocatedTextureBindings;
    std::unordered_map<ImmediateTextureBindingSet, agpu::shader_resource_binding_ref> usedTextureBindingMap;
    size_t usedTextureBindingCount;
};

} // End of namespace AgpuCommon

#endif //AGPU_IMMEDIATE_RENDERER_HPP
