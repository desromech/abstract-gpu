#ifndef AGPU_VULKAN_PLATFORM_HPP
#define AGPU_VULKAN_PLATFORM_HPP

#include "object.hpp"
#include <mutex>

struct _agpu_platform
{
    _agpu_platform();

    void ensureInitialized();

    agpu_icd_dispatch *dispatch;

    std::mutex initializationMutex;
    bool isInitialized;
    bool isSupported;
    uint32_t gpuCount;
};

extern _agpu_platform theVulkanPlatform;

#endif //AGPU_VULKAN_PLATFORM_HPP
