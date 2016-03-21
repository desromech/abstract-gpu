#ifndef AGPU_VULKAN_DEVICE_HPP
#define AGPU_VULKAN_DEVICE_HPP

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#include "object.hpp"
#include <memory>
#include <mutex>
#include <vector>
#include <vulkan/vulkan.h>

/**
* Agpu vulkan device
*/
struct _agpu_device : public Object<_agpu_device>
{
public:
    _agpu_device();

    void lostReferences();

    static agpu_device *open(agpu_device_open_info* openInfo);
    bool initialize(agpu_device_open_info* openInfo);

    agpu_command_queue* getDefaultCommandQueue();
    agpu_command_queue* getGraphicsCommandQueue(agpu_uint index);
    agpu_command_queue* getComputeCommandQueue(agpu_uint index);
    agpu_command_queue* getTransferCommandQueue(agpu_uint index);
    agpu_int getMultiSampleQualityLevels(agpu_uint sample_count);

public:
    std::vector<VkPhysicalDevice> physicalDevices;
    std::vector<VkLayerProperties> instanceLayerProperties;
    std::vector<VkLayerProperties> deviceLayerProperties;
    std::vector<VkExtensionProperties> instanceExtensionProperties;
    std::vector<VkExtensionProperties> deviceExtensionProperties;
    std::vector<VkQueueFamilyProperties> queueProperties;

    VkInstance vulkanInstance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceMemoryProperties memoryProperties;

    PFN_vkGetDeviceProcAddr fpGetDeviceProcAddr;

    // Required extension pointers.
    PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR;
    PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR;
    PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR;
    PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR;
    PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR;
    PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR;
    PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR;
    PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR;
    PFN_vkQueuePresentKHR fpQueuePresentKHR;

    // Queues
    std::vector<agpu_command_queue*> graphicsCommandQueues;
    std::vector<agpu_command_queue*> computeCommandQueues;
    std::vector<agpu_command_queue*> transferCommandQueues;

public:
    bool findMemoryType(uint32_t typeBits, VkFlags requirementsMask, uint32_t *typeIndex)
    {
        // Function taken from the vulkan SDK.
        // Search memtypes to find first index with those properties
        for (uint32_t i = 0; i < 32; i++) {
            if ((typeBits & 1) == 1) {
                // Type is available, does it match user properties?
                if ((memoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask) {
                    *typeIndex = i;
                    return true;
                }
            }
            typeBits >>= 1;
        }

        // No memory types matched, return failure
        return false;
    }

    bool setImageLayout(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout sourceLayout, VkImageLayout destLayout);
    VkImageMemoryBarrier barrierForImageLayoutTransition(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout sourceLayout, VkImageLayout destLayout);

private:
    bool createSetupCommandBuffer();
    bool submitSetupCommandBuffer();

    std::mutex setupMutex;
    VkCommandPool setupCommandPool;
    VkCommandBuffer setupCommandBuffer;
    agpu_command_queue *setupQueue;
};
#endif //AGPU_VULKAN_DEVICE_HPP
