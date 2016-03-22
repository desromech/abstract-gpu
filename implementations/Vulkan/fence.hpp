#ifndef AGPU_VULKAN_FENCE_HPP
#define AGPU_VULKAN_FENCE_HPP

#include "device.hpp"

struct _agpu_fence : public Object<_agpu_fence>
{
    _agpu_fence(agpu_device *device);
    void lostReferences();

    static agpu_fence *create(agpu_device *device);

    agpu_error waitOnClient();

    agpu_device *device;
    VkFence fence;
};

#endif //AGPU_VULKAN_FENCE_HPP
