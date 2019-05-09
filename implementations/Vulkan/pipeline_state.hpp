#ifndef AGPU_VULKAN_PIPELINE_STATE_HPP
#define AGPU_VULKAN_PIPELINE_STATE_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkPipelineState : public agpu::pipeline_state
{
public:
    AVkPipelineState(const agpu::device_ref &device);
    ~AVkPipelineState();

    agpu_int getUniformLocation(agpu_cstring name);

    agpu::device_ref device;
    VkPipeline pipeline;
    VkRenderPass renderPass;
	VkPipelineBindPoint bindPoint;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_PIPELINE_STATE_HPP
