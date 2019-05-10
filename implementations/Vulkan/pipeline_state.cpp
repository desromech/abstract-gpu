#include "pipeline_state.hpp"

namespace AgpuVulkan
{

AVkPipelineState::AVkPipelineState(const agpu::device_ref &device)
    : device(device)
{
    pipeline = VK_NULL_HANDLE;
	renderPass = VK_NULL_HANDLE;
}

AVkPipelineState::~AVkPipelineState()
{
    vkDestroyPipeline(deviceForVk->device, pipeline, nullptr);
	if(renderPass != VK_NULL_HANDLE)
		vkDestroyRenderPass(deviceForVk->device, renderPass, nullptr);
}

agpu_int AVkPipelineState::getUniformLocation(agpu_cstring name)
{
    return -1;
}

} // End of namespace AgpuVulkan
