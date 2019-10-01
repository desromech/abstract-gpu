#include "pipeline_builder.hpp"
#include "pipeline_state.hpp"
#include "shader.hpp"
#include "shader_signature.hpp"
#include "vertex_layout.hpp"
#include "texture_format.hpp"
#include "constants.hpp"

namespace AgpuVulkan
{

AVkGraphicsPipelineBuilder::AVkGraphicsPipelineBuilder(const agpu::device_ref &device)
    : device(device)
{
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

AVkGraphicsPipelineBuilder::~AVkGraphicsPipelineBuilder()
{
    for(auto &stage : stages)
        free((void*)stage.pName);
}

agpu::pipeline_builder_ref AVkGraphicsPipelineBuilder::create(const agpu::device_ref &device)
{
    return agpu::makeObject<AVkGraphicsPipelineBuilder> (device);
}

agpu::pipeline_state_ptr AVkGraphicsPipelineBuilder::build()
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
    auto error = vkCreateRenderPass(deviceForVk->device, &renderPassCreateInfo, nullptr, &renderPass);
    if (error)
        return nullptr;

    pipelineInfo.stageCount = (uint32_t)stages.size();
    pipelineInfo.pStages = &stages[0];
    pipelineInfo.renderPass = renderPass;

    VkPipeline pipeline;
    error = vkCreateGraphicsPipelines(deviceForVk->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);
    if (error)
    {
        vkDestroyRenderPass(deviceForVk->device, renderPass, nullptr);
        return nullptr;
    }

    auto result = agpu::makeObject<AVkPipelineState> (device);
    auto avkPipeline = result.as<AVkPipelineState> ();
    avkPipeline->pipeline = pipeline;
    avkPipeline->renderPass = renderPass;
	avkPipeline->bindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    return result.disown();
}

agpu_error AVkGraphicsPipelineBuilder::attachShader(const agpu::shader_ref &shader)
{
    CHECK_POINTER(shader);

    return attachShaderWithEntryPoint(shader, shader.as<AVkShader> ()->type, "main");
}

agpu_error AVkGraphicsPipelineBuilder::attachShaderWithEntryPoint (const agpu::shader_ref &shader, agpu_shader_type type, agpu_cstring entry_point )
{
    CHECK_POINTER(shader);
    CHECK_POINTER(entry_point);
    auto avkShader = shader.as<AVkShader> ();
    if (!avkShader->shaderModule || type == AGPU_LIBRARY_SHADER)
        return AGPU_INVALID_PARAMETER;

    VkPipelineShaderStageCreateInfo stageInfo;
    memset(&stageInfo, 0, sizeof(stageInfo));
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.module = avkShader->shaderModule;
    stageInfo.pName = duplicateCString(entry_point);
    stageInfo.stage = mapShaderType(type);

    stages.push_back(stageInfo);
    shaders.push_back(shader);
    return AGPU_OK;
}

agpu_size AVkGraphicsPipelineBuilder::getBuildingLogLength()
{
    return 0;
}

agpu_error AVkGraphicsPipelineBuilder::getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer)
{
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setBlendState(agpu_int renderTargetMask, agpu_bool enabled)
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

agpu_error AVkGraphicsPipelineBuilder::setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation)
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

agpu_error AVkGraphicsPipelineBuilder::setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled)
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

agpu_error AVkGraphicsPipelineBuilder::setFrontFace ( agpu_face_winding winding )
{
    rasterizationState.frontFace = mapFaceWinding(winding);
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setCullMode ( agpu_cull_mode mode )
{
    rasterizationState.cullMode = mapCullMode(mode);
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setDepthBias ( agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor )
{
    rasterizationState.depthBiasEnable = VK_TRUE;
    rasterizationState.depthBiasConstantFactor = constant_factor;
    rasterizationState.depthBiasClamp = clamp;
    rasterizationState.depthBiasSlopeFactor = slope_factor;
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function)
{
    depthStencilState.depthTestEnable = enabled ? VK_TRUE : VK_FALSE;
    depthStencilState.depthWriteEnable = writeMask ? VK_TRUE : VK_FALSE;
    depthStencilState.depthCompareOp = mapCompareFunction(function);
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask)
{
    depthStencilState.stencilTestEnable = enabled ? VK_TRUE : VK_FALSE;
    depthStencilState.front.writeMask = writeMask;
    depthStencilState.front.compareMask = readMask;
    depthStencilState.back.writeMask = writeMask;
    depthStencilState.back.compareMask = readMask;
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    depthStencilState.front.failOp = mapStencilOperation(stencilFailOperation);
    depthStencilState.front.depthFailOp = mapStencilOperation(depthFailOperation);
    depthStencilState.front.passOp = mapStencilOperation(stencilDepthPassOperation);
    depthStencilState.front.compareOp = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction)
{
    depthStencilState.back.failOp = mapStencilOperation(stencilFailOperation);
    depthStencilState.back.depthFailOp = mapStencilOperation(depthFailOperation);
    depthStencilState.back.passOp = mapStencilOperation(stencilDepthPassOperation);
    depthStencilState.back.compareOp = mapCompareFunction(stencilFunction);
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setRenderTargetCount(agpu_int count)
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

agpu_error AVkGraphicsPipelineBuilder::setRenderTargetFormat(agpu_uint index, agpu_texture_format format)
{
    if (index >= renderTargetFormats.size())
        return AGPU_INVALID_PARAMETER;

    renderTargetFormats[index] = format;
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setDepthStencilFormat(agpu_texture_format format)
{
    depthStencilFormat = format;
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setPrimitiveType(agpu_primitive_topology topology)
{
    inputAssemblyState.topology = mapTopology(topology);
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setPolygonMode(agpu_polygon_mode mode)
{
    rasterizationState.polygonMode = mapPolygonMode(mode);
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setVertexLayout(const agpu::vertex_layout_ref &layout)
{
    CHECK_POINTER(layout);
    auto avkLayout = layout.as<AVkVertexLayout> ();

    vertexBindings.clear();
    vertexAttributes.clear();

    vertexBindings.reserve(avkLayout->bufferDimensions.size());

    for (auto &bufferData : avkLayout->bufferDimensions)
    {
        VkVertexInputBindingDescription binding;
        binding.binding = (uint32_t)vertexBindings.size();
        binding.stride = bufferData.size;
        binding.inputRate = bufferData.divisor > 0 ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
        vertexBindings.push_back(binding);
    }

    vertexAttributes.reserve(avkLayout->allAttributes.size());
    for (auto &rawAttribute : avkLayout->allAttributes)
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

agpu_error AVkGraphicsPipelineBuilder::setShaderSignature(const agpu::shader_signature_ref &signature)
{
    CHECK_POINTER(signature);
    pipelineInfo.layout = signature.as<AVkShaderSignature> ()->layout;
    this->shaderSignature = signature;
    return AGPU_OK;
}

agpu_error AVkGraphicsPipelineBuilder::setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality)
{
    multisampleState.rasterizationSamples = mapSampleCount(sample_count);
    return AGPU_OK;
}

} // End of namespace AgpuVulkan
