#ifndef AGPU_PIPELINE_BUILDER_HPP_
#define AGPU_PIPELINE_BUILDER_HPP_

#include "device.hpp"
#include "shader.hpp"
#include <utility>
#include <string>
#include <set>

namespace AgpuGL
{

void processTextureWithSamplerCombinations(const std::set<TextureWithSamplerCombination> &rawTextureSamplerCombinations, const agpu::shader_signature_ref &shaderSignature, TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations);

struct GLGraphicsPipelineBuilder: public agpu::pipeline_builder
{
public:
    GLGraphicsPipelineBuilder();
    ~GLGraphicsPipelineBuilder();

    static agpu::pipeline_builder_ref createBuilder(const agpu::device_ref &device);

    virtual agpu::pipeline_state_ptr build ();

    virtual agpu_error setShaderSignature(const agpu::shader_signature_ref &signature) override;

    virtual agpu_error attachShader(const agpu::shader_ref &shader ) override;
    virtual agpu_error attachShaderWithEntryPoint(const agpu::shader_ref &shader, agpu_shader_type type, agpu_cstring entry_point ) override;

    virtual agpu_size getBuildingLogLength() override;
    virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) override;

    virtual agpu_error setBlendState(agpu_int renderTargetMask, agpu_bool enabled) override;
    virtual agpu_error setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation) override;
    virtual agpu_error setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled) override;
    virtual agpu_error setFrontFace ( agpu_face_winding winding ) override;
    virtual agpu_error setCullMode ( agpu_cull_mode mode ) override;
    virtual agpu_error setDepthBias ( agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor ) override;
    virtual agpu_error setDepthState ( agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function ) override;
    virtual agpu_error setStencilState ( agpu_bool enabled, agpu_int writeMask, agpu_int readMask ) override;
    virtual agpu_error setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;
    virtual agpu_error setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;
    virtual agpu_error setRenderTargetCount ( agpu_int count ) override;
    virtual agpu_error setPolygonMode(agpu_polygon_mode mode) override;
    virtual agpu_error setPrimitiveType(agpu_primitive_topology type) override;
    virtual agpu_error setVertexLayout(const agpu::vertex_layout_ref &layout) override;

    virtual agpu_error setRenderTargetFormat(agpu_uint index, agpu_texture_format format) override;
    virtual agpu_error setDepthStencilFormat(agpu_texture_format format) override;
    virtual agpu_error setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality) override;

    agpu_error reset();

public:
    agpu::device_ref device;

    // States
    agpu_bool depthEnabled;
    agpu_bool depthWriteMask;
    agpu_compare_function depthFunction;

    // Depth biasing
    agpu_bool depthBiasEnabled;
    agpu_float depthBiasConstantFactor;
    agpu_float depthBiasClamp;
    agpu_float depthBiasSlopeFactor;

    // Face culling
    agpu_face_winding frontFaceWinding;
    agpu_cull_mode cullingMode;

    // Polgons
    agpu_polygon_mode polygonMode;

    // Render targets
    std::vector<agpu_texture_format> renderTargetFormats;
    agpu_texture_format depthStencilFormat;

    // Color buffer
    agpu_bool blendingEnabled;
    agpu_bool redMask;
    agpu_bool greenMask;
    agpu_bool blueMask;
    agpu_bool alphaMask;
    agpu_blending_factor sourceBlendFactor;
    agpu_blending_factor destBlendFactor;
    agpu_blending_operation blendOperation;
    agpu_blending_factor sourceBlendFactorAlpha;
    agpu_blending_factor destBlendFactorAlpha;
    agpu_blending_operation blendOperationAlpha;

    // Stencil testing
    agpu_bool stencilEnabled;
    int stencilWriteMask;
    int stencilReadMask;

    agpu_stencil_operation stencilFrontFailOp;
    agpu_stencil_operation stencilFrontDepthFailOp;
    agpu_stencil_operation stencilFrontDepthPassOp;
    agpu_compare_function stencilFrontFunc;

    agpu_stencil_operation stencilBackFailOp;
    agpu_stencil_operation stencilBackDepthFailOp;
    agpu_stencil_operation stencilBackDepthPassOp;
    agpu_compare_function stencilBackFunc;

    // Multisampling
    agpu_uint sampleCount;
    agpu_uint sampleQuality;

    // Miscellaneos
    agpu_primitive_topology primitiveType;

    // Error messages
    std::string errorMessages;

    agpu::shader_signature_ref shaderSignature;
    std::vector<std::pair<agpu::shader_ref, std::string>> shaders;

private:
    void buildTextureWithSampleCombinationMapInto(TextureWithSamplerCombinationMap &map, std::vector<MappedTextureWithSamplerCombination> &usedCombinations);
};

} // End of namespace AgpuGL

#endif //AGPU_PIPELINE_BUILDER_HPP_
