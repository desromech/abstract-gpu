#ifndef AGPU_VULKAN_PIPELINE_BUILDER_HPP
#define AGPU_VULKAN_PIPELINE_BUILDER_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkGraphicsPipelineBuilder : public agpu::pipeline_builder
{
public:
    AVkGraphicsPipelineBuilder(const agpu::device_ref &device);
    ~AVkGraphicsPipelineBuilder();

    static agpu::pipeline_builder_ref create(const agpu::device_ref &device);

    virtual agpu::pipeline_state_ptr build() override;
    virtual agpu_error attachShader(const agpu::shader_ref &shader) override;
    virtual agpu_error attachShaderWithEntryPoint(const agpu::shader_ref &shader, agpu_shader_type type, agpu_cstring entry_point) override;
    virtual agpu_size getBuildingLogLength() override;
    virtual agpu_error getBuildingLog(agpu_size buffer_size, agpu_string_buffer buffer) override;
    virtual agpu_error setBlendState(agpu_int renderTargetMask, agpu_bool enabled) override;
    virtual agpu_error setBlendFunction(agpu_int renderTargetMask, agpu_blending_factor sourceFactor, agpu_blending_factor destFactor, agpu_blending_operation colorOperation, agpu_blending_factor sourceAlphaFactor, agpu_blending_factor destAlphaFactor, agpu_blending_operation alphaOperation) override;
    virtual agpu_error setColorMask(agpu_int renderTargetMask, agpu_bool redEnabled, agpu_bool greenEnabled, agpu_bool blueEnabled, agpu_bool alphaEnabled) override;
    virtual agpu_error setFrontFace(agpu_face_winding winding ) override;
    virtual agpu_error setDepthBias(agpu_float constant_factor, agpu_float clamp, agpu_float slope_factor) override;
    virtual agpu_error setCullMode(agpu_cull_mode mode ) override;
    virtual agpu_error setDepthState(agpu_bool enabled, agpu_bool writeMask, agpu_compare_function function) override;
    virtual agpu_error setStencilState(agpu_bool enabled, agpu_int writeMask, agpu_int readMask) override;
    virtual agpu_error setStencilFrontFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;
    virtual agpu_error setStencilBackFace(agpu_stencil_operation stencilFailOperation, agpu_stencil_operation depthFailOperation, agpu_stencil_operation stencilDepthPassOperation, agpu_compare_function stencilFunction) override;
    virtual agpu_error setRenderTargetCount(agpu_int count) override;
    virtual agpu_error setRenderTargetFormat(agpu_uint index, agpu_texture_format format) override;
    virtual agpu_error setDepthStencilFormat(agpu_texture_format format) override;
    virtual agpu_error setPrimitiveType(agpu_primitive_topology type) override;
    virtual agpu_error setPolygonMode(agpu_polygon_mode mode) override;
    virtual agpu_error setVertexLayout(const agpu::vertex_layout_ref &layout) override;
    virtual agpu_error setShaderSignature(const agpu::shader_signature_ref &signature) override;
    virtual agpu_error setSampleDescription(agpu_uint sample_count, agpu_uint sample_quality) override;

    agpu::device_ref device;

private:
    std::vector<agpu::shader_ref> shaders;
    std::vector<VkPipelineShaderStageCreateInfo> stages;
    std::vector<VkDynamicState> dynamicStates;

    std::vector<VkVertexInputBindingDescription> vertexBindings;
    std::vector<VkVertexInputAttributeDescription> vertexAttributes;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachmentState;

    std::vector<agpu_texture_format> renderTargetFormats;
    agpu_texture_format depthStencilFormat;

    VkPipelineVertexInputStateCreateInfo vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    VkPipelineTessellationStateCreateInfo tessellationState;
    VkPipelineViewportStateCreateInfo viewportState;
    VkPipelineRasterizationStateCreateInfo rasterizationState;
    VkPipelineMultisampleStateCreateInfo multisampleState;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    VkPipelineColorBlendStateCreateInfo colorBlendState;
    VkPipelineDynamicStateCreateInfo dynamicState;

    VkGraphicsPipelineCreateInfo pipelineInfo;
    agpu::shader_signature_ref shaderSignature;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_PIPELINE_BUILDER_HPP
