#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "vertex_layout.hpp"

inline D3D12_COMPARISON_FUNC mapCompareFunction(agpu_compare_function function)
{
    switch (function)
    {
    case AGPU_ALWAYS: return D3D12_COMPARISON_FUNC_ALWAYS;
    case AGPU_NEVER: return D3D12_COMPARISON_FUNC_NEVER;
    case AGPU_LESS:  return D3D12_COMPARISON_FUNC_LESS;
    case AGPU_LESS_EQUAL: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
    case AGPU_EQUAL: return D3D12_COMPARISON_FUNC_EQUAL;
    case AGPU_NOT_EQUAL:  return D3D12_COMPARISON_FUNC_NOT_EQUAL;
    case AGPU_GREATER: return D3D12_COMPARISON_FUNC_GREATER;
    case AGPU_GREATER_EQUAL: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
    default:
        abort();
    }
}

inline D3D12_STENCIL_OP mapStencilOperation(agpu_stencil_operation operation)
{
    switch (operation)
    {
    case AGPU_KEEP: return D3D12_STENCIL_OP_KEEP;
    case AGPU_ZERO: return D3D12_STENCIL_OP_ZERO;
    case AGPU_REPLACE: return D3D12_STENCIL_OP_REPLACE;
    case AGPU_INVERT: return D3D12_STENCIL_OP_INVERT;
    case AGPU_INCREASE: return D3D12_STENCIL_OP_INCR_SAT;
    case AGPU_INCREASE_WRAP: return D3D12_STENCIL_OP_INCR;
    case AGPU_DECREASE: return D3D12_STENCIL_OP_DECR_SAT;
    case AGPU_DECREASE_WRAP: return D3D12_STENCIL_OP_DECR;
    default:
        abort();
    }
}

inline D3D12_PRIMITIVE_TOPOLOGY_TYPE mapPrimitiveType(agpu_primitive_type type)
{
    switch (type)
    {
    case AGPU_PRIMITIVE_TYPE_POINT: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case AGPU_PRIMITIVE_TYPE_LINE: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case AGPU_PRIMITIVE_TYPE_TRIANGLE: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case AGPU_PRIMITIVE_TYPE_PATCH: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    default:
        abort();
    }
}

inline D3D12_BLEND mapBlendingFactor(agpu_blending_factor factor)
{
    switch (factor)
    {
    case AGPU_BLENDING_ZERO: return D3D12_BLEND_ZERO;
    case AGPU_BLENDING_ONE: return D3D12_BLEND_ONE;
    case AGPU_BLENDING_SRC_COLOR: return D3D12_BLEND_SRC_COLOR;
    case AGPU_BLENDING_INVERTED_SRC_COLOR: return D3D12_BLEND_INV_SRC_COLOR;
    case AGPU_BLENDING_SRC_ALPHA: return D3D12_BLEND_SRC_ALPHA;
    case AGPU_BLENDING_INVERTED_SRC_ALPHA: return D3D12_BLEND_INV_SRC_ALPHA;
    case AGPU_BLENDING_DEST_ALPHA: return D3D12_BLEND_DEST_ALPHA;
    case AGPU_BLENDING_INVERTED_DEST_ALPHA: return D3D12_BLEND_INV_DEST_ALPHA;
    case AGPU_BLENDING_DEST_COLOR: return D3D12_BLEND_DEST_COLOR;
    case AGPU_BLENDING_INVERTED_DEST_COLOR: return D3D12_BLEND_INV_DEST_COLOR;
    case AGPU_BLENDING_SRC_ALPHA_SAT: return D3D12_BLEND_SRC_ALPHA_SAT;
    case AGPU_BLENDING_CONSTANT_FACTOR: return D3D12_BLEND_BLEND_FACTOR;
    case AGPU_BLENDING_INVERTED_CONSTANT_FACTOR: return D3D12_BLEND_INV_BLEND_FACTOR;
    case AGPU_BLENDING_SRC_1COLOR: return D3D12_BLEND_SRC1_COLOR;
    case AGPU_BLENDING_INVERTED_SRC_1COLOR: return D3D12_BLEND_INV_SRC1_COLOR;
    case AGPU_BLENDING_SRC_1ALPHA: return D3D12_BLEND_SRC1_ALPHA;
    case AGPU_BLENDING_INVERTED_SRC_1ALPHA: return D3D12_BLEND_INV_SRC1_ALPHA;
    default:
        abort();
    }
}

inline D3D12_BLEND_OP mapBlendingOperation(agpu_blending_operation operation)
{
    switch (operation)
    {
    case AGPU_BLENDING_OPERATION_ADD: return D3D12_BLEND_OP_ADD;
    case AGPU_BLENDING_OPERATION_SUBTRACT: return D3D12_BLEND_OP_SUBTRACT;
    case AGPU_BLENDING_OPERATION_REVERSE_SUBTRACT: return D3D12_BLEND_OP_REV_SUBTRACT;
    case AGPU_BLENDING_OPERATION_MIN: return D3D12_BLEND_OP_MIN;
    case AGPU_BLENDING_OPERATION_MAX: return D3D12_BLEND_OP_MAX;
    default:
        abort();
    }
}

_agpu_pipeline_builder::_agpu_pipeline_builder()
{
    memset(&description, 0, sizeof(description));

    // Set default rasterizer state
    description.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    description.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    description.RasterizerState.FrontCounterClockwise = FALSE;
    description.RasterizerState.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
    description.RasterizerState.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
    description.RasterizerState.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
    description.RasterizerState.DepthClipEnable = TRUE;
    description.RasterizerState.MultisampleEnable = FALSE;
    description.RasterizerState.AntialiasedLineEnable = FALSE;
    description.RasterizerState.ForcedSampleCount = 0;
    description.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    // Set the default blend state.
    description.BlendState.AlphaToCoverageEnable = FALSE;
    description.BlendState.IndependentBlendEnable = FALSE;
    const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
    {
        FALSE,FALSE,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
        D3D12_LOGIC_OP_NOOP,
        D3D12_COLOR_WRITE_ENABLE_ALL,
    };
    for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
        description.BlendState.RenderTarget[i] = defaultRenderTargetBlendDesc;

    // Set the depth stencil state
    description.DepthStencilState.DepthEnable = FALSE;
    description.DepthStencilState.StencilEnable = FALSE;

    // Set other defaults.
    description.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    description.NumRenderTargets = 1;
    description.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    description.DSVFormat = DXGI_FORMAT_D16_UNORM;
    description.SampleMask = UINT_MAX;
    description.SampleDesc.Count = 1;
    description.SampleDesc.Quality = 0;

    vertexLayout = nullptr;
}

agpu_error _agpu_pipeline_builder::setShaderSignature(agpu_shader_signature* signature)
{
    CHECK_POINTER(signature);
    rootSignature = signature->rootSignature;
    description.pRootSignature = rootSignature.Get();
    return AGPU_OK;
}

void _agpu_pipeline_builder::lostReferences()
{
    if (vertexLayout)
        vertexLayout->release();
}

_agpu_pipeline_builder *_agpu_pipeline_builder::create(agpu_device *device)
{
    auto builder = new agpu_pipeline_builder();
    builder->device = device;
    return builder;
}

agpu_pipeline_state* _agpu_pipeline_builder::buildPipelineState()
{
    // Build the pipeline
    ComPtr<ID3D12PipelineState> pipelineState;

    if (FAILED(device->d3dDevice->CreateGraphicsPipelineState(&description, IID_PPV_ARGS(&pipelineState))))
    {
        buildingLog = "Failed to create graphics pipeline.";
        return nullptr;
    }

    // Create the wrapper
    auto pipeline = new agpu_pipeline_state();
    pipeline->device = device;
    pipeline->state = pipelineState;
    return pipeline;
}

agpu_error _agpu_pipeline_builder::attachShader(agpu_shader* shader)
{
    CHECK_POINTER(shader);
    switch (shader->type)
    {
    case AGPU_VERTEX_SHADER:
        description.VS = shader->getShaderBytecode();
        break;
    case AGPU_FRAGMENT_SHADER:
        description.PS = shader->getShaderBytecode();
        break;
    case AGPU_GEOMETRY_SHADER:
        description.GS = shader->getShaderBytecode();
        break;
    case AGPU_COMPUTE_SHADER:
        return AGPU_UNSUPPORTED; // It goes in the compute pipeline
    case AGPU_TESSELLATION_CONTROL_SHADER:
        description.HS = shader->getShaderBytecode();
        break;
    case AGPU_TESSELLATION_EVALUATION_SHADER:
        description.DS = shader->getShaderBytecode();
        break;
    }

    return AGPU_OK;
}

agpu_size _agpu_pipeline_builder::getPipelineBuildingLogLength()
{
    return buildingLog.size();
}

agpu_error _agpu_pipeline_builder::getPipelineBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    strncpy(buffer, buildingLog.c_str(), buffer_size);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
    {
        if ((renderTargetMask & (1<<i)) == 0)
            continue;
        description.BlendState.RenderTarget[i].BlendEnable = enabled ? TRUE : FALSE;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{

    for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
    {
        if ((renderTargetMask & (1<<i)) == 0)
            continue;
        auto &blend = description.BlendState.RenderTarget[i];
        blend.SrcBlend = mapBlendingFactor(sourceFactor);
        blend.DestBlend = mapBlendingFactor(destFactor);
        blend.BlendOp = mapBlendingOperation(colorOperation);

        blend.SrcBlendAlpha = mapBlendingFactor(sourceAlphaFactor);
        blend.DestBlendAlpha = mapBlendingFactor(destAlphaFactor);
        blend.BlendOpAlpha= mapBlendingOperation(alphaOperation);
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    int mask = 0;
    if (redEnabled)
        mask |= D3D12_COLOR_WRITE_ENABLE_RED;
    if (greenEnabled)
        mask |= D3D12_COLOR_WRITE_ENABLE_GREEN;
    if (blueEnabled)
        mask |= D3D12_COLOR_WRITE_ENABLE_BLUE;
    if (alphaEnabled)
        mask |= D3D12_COLOR_WRITE_ENABLE_ALPHA;
    auto maskEnum = D3D12_COLOR_WRITE_ENABLE(mask);
    for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
    {
        if ((renderTargetMask & (1 << i)) == 0)
            continue;
        description.BlendState.RenderTarget[i].RenderTargetWriteMask = maskEnum;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    description.DepthStencilState.DepthEnable = enabled;
    description.DepthStencilState.DepthWriteMask = writeMask ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ALL;
    description.DepthStencilState.DepthFunc = mapCompareFunction(function);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    description.DepthStencilState.StencilEnable = enabled;
    description.DepthStencilState.StencilWriteMask = writeMask;
    description.DepthStencilState.StencilReadMask = readMask;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    description.DepthStencilState.FrontFace.StencilFailOp = mapStencilOperation(stencilFailOperation);
    description.DepthStencilState.FrontFace.StencilDepthFailOp = mapStencilOperation(depthFailOperation);
    description.DepthStencilState.FrontFace.StencilPassOp = mapStencilOperation(stencilDepthPassOperation);
    description.DepthStencilState.FrontFace.StencilFunc = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    description.DepthStencilState.BackFace.StencilFailOp = mapStencilOperation(stencilFailOperation);
    description.DepthStencilState.BackFace.StencilDepthFailOp = mapStencilOperation(depthFailOperation);
    description.DepthStencilState.BackFace.StencilPassOp = mapStencilOperation(stencilDepthPassOperation);
    description.DepthStencilState.BackFace.StencilFunc = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetCount(agpu_int count)
{
    memset(description.RTVFormats, 0, sizeof(description.RTVFormats));
    description.NumRenderTargets = count;
    for (int i = 0; i < count; ++i)
        description.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    description.RTVFormats[index] = (DXGI_FORMAT)format;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthStencilFormat(agpu_texture_format format)
{
    description.DSVFormat = (DXGI_FORMAT)format;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPrimitiveType(agpu_primitive_type type)
{
    description.PrimitiveTopologyType = mapPrimitiveType(type);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setVertexLayout(agpu_vertex_layout* layout)
{
    layout->retain();
    if(vertexLayout)
        vertexLayout->release();
    vertexLayout = layout;

    description.InputLayout.NumElements = (UINT)vertexLayout->inputElements.size();
    description.InputLayout.pInputElementDescs = &vertexLayout->inputElements[0];
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    description.SampleDesc.Count = sample_count;
    description.SampleDesc.Quality = sample_quality;
    if (sample_count > 1)
    {
        description.RasterizerState.MultisampleEnable = TRUE;
        description.RasterizerState.AntialiasedLineEnable = FALSE;
    }
    else
    {
        description.RasterizerState.MultisampleEnable = FALSE;
        description.RasterizerState.AntialiasedLineEnable = FALSE;
    }
    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddPipelineBuilderReference(agpu_pipeline_builder* pipeline_builder)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleasePipelineBuilder(agpu_pipeline_builder* pipeline_builder)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->release();
}

AGPU_EXPORT agpu_pipeline_state* agpuBuildPipelineState(agpu_pipeline_builder* pipeline_builder)
{
    if (!pipeline_builder)
        return nullptr;
    return pipeline_builder->buildPipelineState();
}

AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature(agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setShaderSignature(signature);
}

AGPU_EXPORT agpu_error agpuAttachShader(agpu_pipeline_builder* pipeline_builder, agpu_shader* shader)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->attachShader(shader);
}

AGPU_EXPORT agpu_size agpuGetPipelineBuildingLogLength(agpu_pipeline_builder* pipeline_builder)
{
    if (!pipeline_builder)
        return 0;
    return pipeline_builder->getPipelineBuildingLogLength();
}

AGPU_EXPORT agpu_error agpuGetPipelineBuildingLog(agpu_pipeline_builder* pipeline_builder, agpu_size buffer_size, agpu_string_buffer buffer)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->getPipelineBuildingLog(buffer_size, buffer);
}

AGPU_EXPORT agpu_error agpuSetBlendState(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool enabled)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setBlendState(renderTargetMask, enabled);
}

AGPU_EXPORT agpu_error agpuSetBlendFunction(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setBlendFunction(renderTargetMask, sourceFactor, destFactor, colorOperation, sourceAlphaFactor, destAlphaFactor, alphaOperation);
}

AGPU_EXPORT agpu_error agpuSetColorMask(agpu_pipeline_builder* pipeline_builder, agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setColorMask(renderTargetMask, redEnabled, greenEnabled, blueEnabled, alphaEnabled);
}

AGPU_EXPORT agpu_error agpuSetDepthState(agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setDepthState(enabled, writeMask, function);
}

AGPU_EXPORT agpu_error agpuSetStencilState(agpu_pipeline_builder* pipeline_builder, agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilState(enabled, writeMask, readMask);
}

AGPU_EXPORT agpu_error agpuSetStencilFrontFace(agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilFrontFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetStencilBackFace(agpu_pipeline_builder* pipeline_builder, agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setStencilBackFace(stencilFailOperation, depthFailOperation, stencilDepthPassOperation, stencilFunction);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetCount(agpu_pipeline_builder* pipeline_builder, agpu_int count)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setRenderTargetCount(count);
}

AGPU_EXPORT agpu_error agpuSetRenderTargetFormat(agpu_pipeline_builder* pipeline_builder, agpu_uint index, agpu_texture_format format)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setRenderTargetFormat(index, format);
}

AGPU_EXPORT agpu_error agpuSetDepthStencilFormat(agpu_pipeline_builder* pipeline_builder, agpu_texture_format format)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setDepthStencilFormat(format);
}

AGPU_EXPORT agpu_error agpuSetPrimitiveType(agpu_pipeline_builder* pipeline_builder, agpu_primitive_type type)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPrimitiveType(type);
}

AGPU_EXPORT agpu_error agpuSetVertexLayout(agpu_pipeline_builder* pipeline_builder, agpu_vertex_layout* layout)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setVertexLayout(layout);
}

AGPU_EXPORT agpu_error agpuSetSampleDescription(agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setSampleDescription(sample_count, sample_quality);
}
