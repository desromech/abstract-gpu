#include "pipeline_builder.hpp"

_agpu_pipeline_builder::_agpu_pipeline_builder(agpu_device *device)
    : device(device)
{
    // Default vertex input state.
    memset(&vertexInputState, 0, sizeof(vertexInputState));
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    // Default input assembly state.
    memset(&inputAssemblyState, 0, sizeof(inputAssemblyState));
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    // Default tessellation state.
    memset(&tessellationState, 0, sizeof(tessellationState));
    tessellationState.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    // Default viewport state.
    memset(&viewportState, 0, sizeof(viewportState));
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;

    // Default rasterization state.
    memset(&rasterizationState, 0, sizeof(rasterizationState));
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

    // Default multisample state.
    memset(&multisampleState, 0, sizeof(multisampleState));
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;

    // Default depth stencil state.
    memset(&depthStencilState, 0, sizeof(depthStencilState));
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Default color blend state.
    memset(&colorBlendState, 0, sizeof(colorBlendState));
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    // Default dynamic state.
    memset(&dynamicState, 0, sizeof(dynamicState));
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    // Pipeline state info.
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = &dynamicState;

}

void _agpu_pipeline_builder::lostReferences()
{
}

_agpu_pipeline_builder *_agpu_pipeline_builder::create(agpu_device *device)
{
    std::unique_ptr<_agpu_pipeline_builder> builder(new _agpu_pipeline_builder(device));
    return builder.release();
}

agpu_pipeline_state* _agpu_pipeline_builder::buildPipelineState()
{
    if (!stages.empty())
    {
        pipelineInfo.stageCount = stages.size();
        pipelineInfo.pStages = &stages[0];
    }

    VkPipeline pipeline;
    auto error = vkCreateGraphicsPipelines(device->device, nullptr, 1, &pipelineInfo, nullptr, &pipeline);
    if (error)
        return nullptr;

    return nullptr;
}

agpu_error _agpu_pipeline_builder::attachShader(agpu_shader* shader)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_size _agpu_pipeline_builder::getPipelineBuildingLogLength()
{
    return 0;
}

agpu_error _agpu_pipeline_builder::getPipelineBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setRenderTargetCount(agpu_int count)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setDepthStencilFormat(agpu_texture_format format)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setPrimitiveType(agpu_primitive_type type)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setVertexLayout(agpu_vertex_layout* layout)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setPipelineShaderSignature(agpu_shader_signature* signature)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_pipeline_builder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface
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

AGPU_EXPORT agpu_error agpuSetPipelineShaderSignature(agpu_pipeline_builder* pipeline_builder, agpu_shader_signature* signature)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPipelineShaderSignature(signature);
}

AGPU_EXPORT agpu_error agpuSetSampleDescription(agpu_pipeline_builder* pipeline_builder, agpu_uint sample_count, agpu_uint sample_quality)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setSampleDescription(sample_count, sample_quality);
}
