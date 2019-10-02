#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "vertex_layout.hpp"

namespace AgpuD3D12
{
inline D3D12_CULL_MODE mapCullMode(agpu_cull_mode mode)
{
    switch(mode)
    {
    case AGPU_CULL_MODE_FRONT:
        return D3D12_CULL_MODE_FRONT;
    case AGPU_CULL_MODE_BACK:
        return D3D12_CULL_MODE_BACK;
    case AGPU_CULL_MODE_FRONT_AND_BACK:
    case AGPU_CULL_MODE_NONE:
        return D3D12_CULL_MODE_NONE;
	default: abort();
    }
}

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

inline D3D12_PRIMITIVE_TOPOLOGY_TYPE mapPrimitiveType(agpu_primitive_topology type)
{
    switch (type)
    {
    case AGPU_POINTS:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    case AGPU_LINES:
    case AGPU_LINES_ADJACENCY:
    case AGPU_LINE_STRIP:
    case AGPU_LINE_STRIP_ADJACENCY:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
    case AGPU_TRIANGLES:
    case AGPU_TRIANGLES_ADJACENCY:
    case AGPU_TRIANGLE_STRIP:
    case AGPU_TRIANGLE_STRIP_ADJACENCY:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    case AGPU_PATCHES:
        return D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
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

ADXPipelineBuilder::ADXPipelineBuilder(const agpu::device_ref &cdevice)
    : device(cdevice)
{
    memset(&description, 0, sizeof(description));

    // Set default rasterizer state
    description.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    description.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    description.RasterizerState.FrontCounterClockwise = TRUE;
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
    description.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    description.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    description.SampleMask = UINT_MAX;
    description.SampleDesc.Count = 1;
    description.SampleDesc.Quality = 0;
}

ADXPipelineBuilder::~ADXPipelineBuilder()
{
}

agpu_error ADXPipelineBuilder::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    CHECK_POINTER(signature);
	this->shaderSignature = signature;
    rootSignature = signature.as<ADXShaderSignature> ()->rootSignature;
    description.pRootSignature = rootSignature.Get();
    return AGPU_OK;
}

agpu::pipeline_builder_ref ADXPipelineBuilder::create(const agpu::device_ref &device)
{
    return agpu::makeObject<ADXPipelineBuilder> (device);
}

agpu::pipeline_state_ptr ADXPipelineBuilder::build()
{
    // Build the pipeline
    ComPtr<ID3D12PipelineState> pipelineState;

    if (FAILED(deviceForDX->d3dDevice->CreateGraphicsPipelineState(&description, IID_PPV_ARGS(&pipelineState))))
    {
        buildingLog = "Failed to create graphics pipeline.";
        return nullptr;
    }

    // Create the wrapper
    auto pipeline = agpu::makeObject<ADXPipelineState> ();
    auto adxPipeline = pipeline.as<ADXPipelineState> ();
    adxPipeline->device = device;
    adxPipeline->state = pipelineState;
    adxPipeline->primitiveTopology = primitiveTopology;
    return pipeline.disown();
}

agpu_error ADXPipelineBuilder::attachShader(const agpu::shader_ref &shader)
{
    CHECK_POINTER(shader);
    auto adxShader = shader.as<ADXShader> ();
    if(adxShader->type == AGPU_LIBRARY_SHADER)
        return AGPU_INVALID_PARAMETER;

    return attachShaderWithEntryPoint(shader, adxShader->type, "main");
}

agpu_error ADXPipelineBuilder::attachShaderWithEntryPoint(const agpu::shader_ref & shader, agpu_shader_type type, agpu_cstring entry_point)
{
    auto adxShader = shader.as<ADXShader> ();
    D3D12_SHADER_BYTECODE *dest = nullptr;
    switch (type)
    {
    case AGPU_VERTEX_SHADER:
		dest = &description.VS;
        break;
    case AGPU_FRAGMENT_SHADER:
        dest = &description.PS;
        break;
    case AGPU_GEOMETRY_SHADER:
        dest = &description.GS;
        break;
    case AGPU_COMPUTE_SHADER:
        return AGPU_UNSUPPORTED; // It goes in the compute pipeline
    case AGPU_TESSELLATION_CONTROL_SHADER:
        dest = &description.HS;
        break;
    case AGPU_TESSELLATION_EVALUATION_SHADER:
        dest = &description.DS;
        break;
    default:
        return AGPU_INVALID_PARAMETER;
    }

    return adxShader->getShaderBytecodeForEntryPoint(shaderSignature, type, entry_point, buildingLog, dest);
}

agpu_size ADXPipelineBuilder::getBuildingLogLength()
{
    return buildingLog.size();
}

agpu_error ADXPipelineBuilder::getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    strncpy(buffer, buildingLog.c_str(), buffer_size);
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    for (int i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
    {
        if ((renderTargetMask & (1<<i)) == 0)
            continue;
        description.BlendState.RenderTarget[i].BlendEnable = enabled ? TRUE : FALSE;
    }

    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
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

agpu_error ADXPipelineBuilder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
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

agpu_error ADXPipelineBuilder::setFrontFace(agpu_face_winding winding)
{
    description.RasterizerState.FrontCounterClockwise = winding == AGPU_COUNTER_CLOCKWISE;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setCullMode(agpu_cull_mode mode)
{
    description.RasterizerState.CullMode = mapCullMode(mode);
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor)
{
    description.RasterizerState.DepthBias = constant_factor;
    description.RasterizerState.DepthBiasClamp = clamp;
    description.RasterizerState.SlopeScaledDepthBias = slope_factor;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    description.DepthStencilState.DepthEnable = enabled;
    description.DepthStencilState.DepthWriteMask = writeMask ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ALL;
    description.DepthStencilState.DepthFunc = mapCompareFunction(function);
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setPolygonMode(agpu_polygon_mode mode)
{
    description.RasterizerState.FillMode = mode == AGPU_POLYGON_MODE_FILL ? D3D12_FILL_MODE_SOLID : D3D12_FILL_MODE_WIREFRAME;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    description.DepthStencilState.StencilEnable = enabled;
    description.DepthStencilState.StencilWriteMask = writeMask;
    description.DepthStencilState.StencilReadMask = readMask;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    description.DepthStencilState.FrontFace.StencilFailOp = mapStencilOperation(stencilFailOperation);
    description.DepthStencilState.FrontFace.StencilDepthFailOp = mapStencilOperation(depthFailOperation);
    description.DepthStencilState.FrontFace.StencilPassOp = mapStencilOperation(stencilDepthPassOperation);
    description.DepthStencilState.FrontFace.StencilFunc = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    description.DepthStencilState.BackFace.StencilFailOp = mapStencilOperation(stencilFailOperation);
    description.DepthStencilState.BackFace.StencilDepthFailOp = mapStencilOperation(depthFailOperation);
    description.DepthStencilState.BackFace.StencilPassOp = mapStencilOperation(stencilDepthPassOperation);
    description.DepthStencilState.BackFace.StencilFunc = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setRenderTargetCount(agpu_int count)
{
    memset(description.RTVFormats, 0, sizeof(description.RTVFormats));
    description.NumRenderTargets = count;
    for (int i = 0; i < count; ++i)
        description.RTVFormats[i] = DXGI_FORMAT_R8G8B8A8_UNORM;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    description.RTVFormats[index] = (DXGI_FORMAT)format;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setDepthStencilFormat(agpu_texture_format format)
{
    description.DSVFormat = (DXGI_FORMAT)format;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setPrimitiveType(agpu_primitive_topology type)
{
    description.PrimitiveTopologyType = mapPrimitiveType(type);
    primitiveTopology = type;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setVertexLayout(const agpu::vertex_layout_ref &layout)
{
    CHECK_POINTER(layout);
    vertexLayout = layout;

    auto adxVertexLayout = layout.as<ADXVertexLayout> ();

    description.InputLayout.NumElements = (UINT)adxVertexLayout->inputElements.size();
    if(!adxVertexLayout->inputElements.empty())
        description.InputLayout.pInputElementDescs = &adxVertexLayout->inputElements[0];
    else
        description.InputLayout.pInputElementDescs = nullptr;
    return AGPU_OK;
}

agpu_error ADXPipelineBuilder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
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

} // End of namespace AgpuD3D12
