#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "vertex_layout.hpp"
#include "texture_format.hpp"
#include "constants.hpp"

_agpu_pipeline_builder::_agpu_pipeline_builder(agpu_device *device)
    : device(device)
{
    shaderSignature = nullptr;

    // Render targets
    renderTargetFormats.resize(1, AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM);
    depthStencilFormat = AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT;

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
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.scissorCount = 1;
    viewportState.viewportCount = 1;

    // Default rasterization state.
    memset(&rasterizationState, 0, sizeof(rasterizationState));
    rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationState.cullMode = VK_CULL_MODE_NONE;
    rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationState.depthClampEnable = VK_FALSE;
    rasterizationState.rasterizerDiscardEnable = VK_FALSE;
    rasterizationState.depthBiasEnable = VK_FALSE;
    rasterizationState.lineWidth = 1.0f;

    // Default multisample state.
    memset(&multisampleState, 0, sizeof(multisampleState));
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    // Default depth stencil state.
    memset(&depthStencilState, 0, sizeof(depthStencilState));
    depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

    // Default color blend state.
    memset(&colorBlendState, 0, sizeof(colorBlendState));
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

    {
        VkPipelineColorBlendAttachmentState state;
        memset(&state, 0, sizeof(state));
        state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        state.blendEnable = VK_FALSE;
        colorBlendAttachmentState.resize(1, state);
    }


    // Set dynamic states.
    dynamicStates.push_back(VK_DYNAMIC_STATE_VIEWPORT);
    dynamicStates.push_back(VK_DYNAMIC_STATE_SCISSOR);
    dynamicStates.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);

    memset(&dynamicState, 0, sizeof(dynamicState));
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = (uint32_t)dynamicStates.size();
    dynamicState.pDynamicStates = &dynamicStates[0];

    // Pipeline state info.
    memset(&pipelineInfo, 0, sizeof(pipelineInfo));
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    pipelineInfo.pVertexInputState = &vertexInputState;
    pipelineInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizationState;
    pipelineInfo.pMultisampleState = &multisampleState;
    pipelineInfo.pDepthStencilState = &depthStencilState;
    pipelineInfo.pColorBlendState = &colorBlendState;
    pipelineInfo.pDynamicState = &dynamicState;

    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;
}

void _agpu_pipeline_builder::lostReferences()
{
    if (shaderSignature)
        shaderSignature->release();
    for (auto shader : shaders)
        shader->release();
    for(auto &stage : stages)
        free((void*)stage.pName);
}

_agpu_pipeline_builder *_agpu_pipeline_builder::create(agpu_device *device)
{
    std::unique_ptr<_agpu_pipeline_builder> builder(new _agpu_pipeline_builder(device));
    return builder.release();
}

agpu_pipeline_state* _agpu_pipeline_builder::buildPipelineState()
{
    if (stages.empty())
        return nullptr;

    // Attachments
    std::vector<VkAttachmentDescription> attachments(renderTargetFormats.size() + (depthStencilFormat != AGPU_TEXTURE_FORMAT_UNKNOWN? 1 : 0));
    for (agpu_uint i = 0; i < renderTargetFormats.size(); ++i)
    {
        auto &attachment = attachments[i];
        attachment.format = mapTextureFormat(renderTargetFormats[i]);
        attachment.samples = multisampleState.rasterizationSamples;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    if (depthStencilFormat != AGPU_TEXTURE_FORMAT_UNKNOWN)
    {
        auto &attachment = attachments.back();
        attachment.format = mapTextureFormat(depthStencilFormat);
        attachment.samples = multisampleState.rasterizationSamples;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    // Color reference
    std::vector<VkAttachmentReference> colorReference(renderTargetFormats.size());
    for (agpu_uint i = 0; i < renderTargetFormats.size(); ++i)
    {
        colorReference[i].attachment = i;
        colorReference[i].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }

    // Depth reference
    VkAttachmentReference depthReference;
    memset(&depthReference, 0, sizeof(depthReference));
    depthReference.attachment = (uint32_t)renderTargetFormats.size();
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Sub pass
    VkSubpassDescription subpass;
    memset(&subpass, 0, sizeof(subpass));
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = (uint32_t)renderTargetFormats.size();
    subpass.pColorAttachments = colorReference.empty() ? nullptr : &colorReference[0];
    subpass.pDepthStencilAttachment = &depthReference;
    if (depthStencilFormat == AGPU_TEXTURE_FORMAT_UNKNOWN)
        subpass.pDepthStencilAttachment = nullptr;

    // Finish the color blend state.
    if (colorBlendAttachmentState.empty())
    {
        colorBlendState.attachmentCount = 0;
        colorBlendState.pAttachments = nullptr;
    }
    else
    {
        colorBlendState.attachmentCount = (uint32_t)colorBlendAttachmentState.size();
        colorBlendState.pAttachments = &colorBlendAttachmentState[0];
    }

    // Render pass
    VkRenderPassCreateInfo renderPassCreateInfo;
    memset(&renderPassCreateInfo, 0, sizeof(renderPassCreateInfo));
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.attachmentCount = (uint32_t)attachments.size();
    renderPassCreateInfo.pAttachments = &attachments[0];
    renderPassCreateInfo.subpassCount = 1;
    renderPassCreateInfo.pSubpasses = &subpass;

    VkRenderPass renderPass;
    auto error = vkCreateRenderPass(device->device, &renderPassCreateInfo, nullptr, &renderPass);
    if (error)
        return nullptr;

    pipelineInfo.stageCount = (uint32_t)stages.size();
    pipelineInfo.pStages = &stages[0];
    pipelineInfo.renderPass = renderPass;

    VkPipeline pipeline;
    error = vkCreateGraphicsPipelines(device->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if (error)
    {
        vkDestroyRenderPass(device->device, renderPass, nullptr);
        return nullptr;
    }

    auto result = new agpu_pipeline_state(device);
    result->pipeline = pipeline;
    result->renderPass = renderPass;
	result->bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return result;
}

agpu_error _agpu_pipeline_builder::attachShader(agpu_shader* shader)
{
    CHECK_POINTER(shader);

    return attachShaderWithEntryPoint(shader, shader->type, "main");
}

agpu_error _agpu_pipeline_builder::attachShaderWithEntryPoint ( agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(shader);
    CHECK_POINTER(entry_point);
    if (!shader->shaderModule || type == AGPU_LIBRARY_SHADER)
        return AGPU_INVALID_PARAMETER;

    VkPipelineShaderStageCreateInfo stageInfo;
    memset(&stageInfo, 0, sizeof(stageInfo));
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.module = shader->shaderModule;
    stageInfo.pName = duplicateCString(entry_point);
    stageInfo.stage = mapShaderType(type);

    shader->retain();
    stages.push_back(stageInfo);
    shaders.push_back(shader);
    return AGPU_OK;
}

agpu_size _agpu_pipeline_builder::getPipelineBuildingLogLength()
{
    return 0;
}

agpu_error _agpu_pipeline_builder::getPipelineBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
{
    for(size_t i = 0; i < colorBlendAttachmentState.size(); ++i)
    {
        if((renderTargetMask & (1 << i)) == 0)
            continue;

        auto &state = colorBlendAttachmentState[i];
        state.blendEnable = enabled ? VK_TRUE : VK_FALSE;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
{
    auto vkSourceFactor = mapBlendingFactor(sourceFactor, true);
    auto vkDestFactor = mapBlendingFactor(destFactor, true);
    auto vkColorOperation = mapBlendingOperation(colorOperation);
    auto vkSourceAlphaFactor = mapBlendingFactor(sourceAlphaFactor, false);
    auto vkDestAlphaFactor = mapBlendingFactor(destAlphaFactor, false);
    auto vkAlphaOperation = mapBlendingOperation(alphaOperation);
    for(size_t i = 0; i < colorBlendAttachmentState.size(); ++i)
    {
        if((renderTargetMask & (1 << i)) == 0)
            continue;

        auto &state = colorBlendAttachmentState[i];
        state.srcColorBlendFactor = vkSourceFactor;
        state.dstColorBlendFactor = vkDestFactor;
        state.colorBlendOp = vkColorOperation;
        state.srcAlphaBlendFactor = vkSourceAlphaFactor;
        state.dstAlphaBlendFactor = vkDestAlphaFactor;
        state.alphaBlendOp = vkAlphaOperation;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
{
    int colorMask = 0;
    if(redEnabled)
        colorMask |= VK_COLOR_COMPONENT_R_BIT;
    if(greenEnabled)
        colorMask |= VK_COLOR_COMPONENT_G_BIT;
    if(blueEnabled)
        colorMask |= VK_COLOR_COMPONENT_B_BIT;
    if(alphaEnabled)
        colorMask |= VK_COLOR_COMPONENT_A_BIT;

    for(size_t i = 0; i < colorBlendAttachmentState.size(); ++i)
    {
        if((renderTargetMask & (1 << i)) == 0)
            continue;

        auto &state = colorBlendAttachmentState[i];
        state.colorWriteMask = colorMask;
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setFrontFace ( agpu_face_winding winding )
{
    rasterizationState.frontFace = mapFaceWinding(winding);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setCullMode ( agpu_cull_mode mode )
{
    rasterizationState.cullMode = mapCullMode(mode);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthBias ( agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
{
    rasterizationState.depthBiasEnable = VK_TRUE;
    rasterizationState.depthBiasConstantFactor = constant_factor;
    rasterizationState.depthBiasClamp = clamp;
    rasterizationState.depthBiasSlopeFactor = slope_factor;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    depthStencilState.depthTestEnable = enabled ? VK_TRUE : VK_FALSE;
    depthStencilState.depthWriteEnable = writeMask ? VK_TRUE : VK_FALSE;
    depthStencilState.depthCompareOp = mapCompareFunction(function);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    depthStencilState.stencilTestEnable = enabled ? VK_TRUE : VK_FALSE;
    depthStencilState.front.writeMask = writeMask;
    depthStencilState.front.compareMask = readMask;
    depthStencilState.back.writeMask = writeMask;
    depthStencilState.back.compareMask = readMask;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    depthStencilState.front.failOp = mapStencilOperation(stencilFailOperation);
    depthStencilState.front.depthFailOp = mapStencilOperation(depthFailOperation);
    depthStencilState.front.passOp = mapStencilOperation(stencilDepthPassOperation);
    depthStencilState.front.compareOp = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    depthStencilState.back.failOp = mapStencilOperation(stencilFailOperation);
    depthStencilState.back.depthFailOp = mapStencilOperation(depthFailOperation);
    depthStencilState.back.passOp = mapStencilOperation(stencilDepthPassOperation);
    depthStencilState.back.compareOp = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetCount(agpu_int count)
{
    renderTargetFormats.resize(count, AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM);

    {
        VkPipelineColorBlendAttachmentState state;
        memset(&state, 0, sizeof(state));
        state.colorWriteMask = 0x1f;
        state.blendEnable = VK_FALSE;
        colorBlendAttachmentState.resize(count, state);
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    if (index >= renderTargetFormats.size())
        return AGPU_INVALID_PARAMETER;

    renderTargetFormats[index] = format;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setDepthStencilFormat(agpu_texture_format format)
{
    depthStencilFormat = format;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPrimitiveType(agpu_primitive_topology topology)
{
    inputAssemblyState.topology = mapTopology(topology);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPolygonMode(agpu_polygon_mode mode)
{
    rasterizationState.polygonMode = mapPolygonMode(mode);
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setVertexLayout(agpu_vertex_layout* layout)
{
    vertexBindings.clear();
    vertexAttributes.clear();

    vertexBindings.reserve(layout->bufferDimensions.size());

    for (auto &bufferData : layout->bufferDimensions)
    {
        VkVertexInputBindingDescription binding;
        binding.binding = (uint32_t)vertexBindings.size();
        binding.stride = bufferData.size;
        binding.inputRate = bufferData.divisor > 0 ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        vertexBindings.push_back(binding);
    }

    vertexAttributes.reserve(layout->allAttributes.size());
    for (auto &rawAttribute : layout->allAttributes)
    {
        VkVertexInputAttributeDescription attribute;
        attribute.binding = rawAttribute.buffer;
        attribute.format = mapTextureFormat(rawAttribute.format);
        attribute.location = rawAttribute.binding;
        attribute.offset = rawAttribute.offset;
        vertexAttributes.push_back(attribute);
    }

    if (vertexBindings.empty())
    {
        vertexInputState.vertexBindingDescriptionCount = 0;
        vertexInputState.pVertexBindingDescriptions = nullptr;
    }
    else
    {
        vertexInputState.vertexBindingDescriptionCount = (uint32_t)vertexBindings.size();
        vertexInputState.pVertexBindingDescriptions = &vertexBindings[0];
    }

    if (vertexAttributes.empty())
    {
        vertexInputState.vertexAttributeDescriptionCount = 0;
        vertexInputState.pVertexAttributeDescriptions = nullptr;
    }
    else
    {
        vertexInputState.vertexAttributeDescriptionCount = (uint32_t)vertexAttributes.size();
        vertexInputState.pVertexAttributeDescriptions = &vertexAttributes[0];
    }

    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setPipelineShaderSignature(agpu_shader_signature* signature)
{
    CHECK_POINTER(signature);
    pipelineInfo.layout = signature->layout;
    signature->retain();
    if (this->shaderSignature)
        this->shaderSignature->release();
    this->shaderSignature = signature;
    return AGPU_OK;
}

agpu_error _agpu_pipeline_builder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    multisampleState.rasterizationSamples = mapSampleCount(sample_count);
    return AGPU_OK;
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

AGPU_EXPORT agpu_error agpuAttachShaderWithEntryPoint ( agpu_pipeline_builder* pipeline_builder, agpu_shader* shader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->attachShaderWithEntryPoint(shader, type, entry_point);
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

AGPU_EXPORT agpu_error agpuSetDepthBias ( agpu_pipeline_builder* pipeline_builder, agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setDepthBias(constant_factor, clamp, slope_factor);
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

AGPU_EXPORT agpu_error agpuSetPrimitiveType(agpu_pipeline_builder* pipeline_builder, agpu_primitive_topology type)
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

AGPU_EXPORT agpu_error agpuSetFrontFace ( agpu_pipeline_builder* pipeline_builder, agpu_face_winding winding )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setFrontFace(winding);
}

AGPU_EXPORT agpu_error agpuSetCullMode ( agpu_pipeline_builder* pipeline_builder, agpu_cull_mode mode )
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setCullMode(mode);
}

AGPU_EXPORT agpu_error agpuSetPolygonMode(agpu_pipeline_builder* pipeline_builder, agpu_polygon_mode mode)
{
    CHECK_POINTER(pipeline_builder);
    return pipeline_builder->setPolygonMode(mode);
}
