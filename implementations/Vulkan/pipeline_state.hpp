#ifndef AGPU_VULKAN_PIPELINE_STATE_HPP
#define AGPU_VULKAN_PIPELINE_STATE_HPP

#include "device.hpp"

struct _agpu_pipeline_state : public Object<_agpu_pipeline_state>
{
    _agpu_pipeline_state(agpu_device *device);
    void lostReferences();

    agpu_int getUniformLocation(agpu_cstring name);

    agpu_device *device;
    VkPipeline pipeline;
    VkRenderPass renderPass;
	VkPipelineBindPoint bindPoint;
};

#endif //AGPU_VULKAN_PIPELINE_STATE_HPP
