#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
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
    description.SampleMask = UINT_MAX;
    description.SampleDesc.Count = 1;

    vertexLayout = nullptr;
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
    // Set the root signature
    description.pRootSignature = device->graphicsRootSignature.Get();

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

agpu_error _agpu_pipeline_builder::setRenderTargetCount(agpu_int count)
{
    description.NumRenderTargets = count;
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

AGPU_EXPORT agpu_error agpuSetRenderTargetCount(agpu_pipeline_builder* pipeline_builder, agpu_int count)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setRenderTargetCount(count);
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