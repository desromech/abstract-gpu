#ifndef AGPU_VULKAN_INCLUDE_VULKAN_H
#define AGPU_VULKAN_INCLUDE_VULKAN_H

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__unix__)
#define VK_USE_PLATFORM_XCB_KHR
#else
#error unsupported platform
#endif

#include <vulkan/vulkan.h>

#define VK_ERROR_OUT_OF_POOL_MEMORY -1000069000 // For VulkanMemoryAllocator on CI

#endif //AGPU_VULKAN_INCLUDE_VULKAN_H
