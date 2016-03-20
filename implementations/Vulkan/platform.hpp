#ifndef AGPU_VULKAN_PLATFORM_HPP
#define AGPU_VULKAN_PLATFORM_HPP

#include "object.hpp"

struct _agpu_platform
{
    agpu_icd_dispatch *dispatch;
};

extern _agpu_platform theVulkanPlatform;

#endif //AGPU_VULKAN_PLATFORM_HPP
