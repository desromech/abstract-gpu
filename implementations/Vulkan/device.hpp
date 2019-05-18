#ifndef AGPU_VULKAN_DEVICE_HPP
#define AGPU_VULKAN_DEVICE_HPP

#include "common.hpp"
#include "include_vulkan.h"
#include <string.h>
#include <memory>
#include <mutex>
#include <vector>
#include <openvr.h>

#define DECLARE_VK_EXTENSION_FP(name) PFN_vk ## name fp ## name

namespace AgpuVulkan
{
class VulkanPlatform;

/**
* Agpu vulkan device
*/
class AVkDevice : public agpu::device
{
public:
    AVkDevice();
    ~AVkDevice();

    static bool checkVulkanImplementation(VulkanPlatform *platform);

    static bool getInstanceExtensionsRequiredForVR(std::vector<std::string> &requiredInstanceExtensions);
    static bool getDeviceExtensionsRequiredForVR(VkPhysicalDevice physicalDevice, std::vector<std::string> &requiredDeviceExtensions);

    static agpu::device_ref open(agpu_device_open_info* openInfo);
    bool initialize(agpu_device_open_info* openInfo);

    virtual agpu::command_queue_ptr getDefaultCommandQueue() override;
    virtual agpu::command_queue_ptr getGraphicsCommandQueue(agpu_uint index);
    virtual agpu::command_queue_ptr getComputeCommandQueue(agpu_uint index);
    virtual agpu::command_queue_ptr getTransferCommandQueue(agpu_uint index);

	virtual agpu::swap_chain_ptr createSwapChain(const agpu::command_queue_ref & commandQueue, agpu_swap_chain_create_info* swapChainInfo) override;
	virtual agpu::buffer_ptr createBuffer(agpu_buffer_description* description, agpu_pointer initial_data) override;
	virtual agpu::vertex_layout_ptr createVertexLayout() override;
	virtual agpu::vertex_binding_ptr createVertexBinding(const agpu::vertex_layout_ref & layout) override;
	virtual agpu::shader_ptr createShader(agpu_shader_type type) override;
	virtual agpu::shader_signature_builder_ptr createShaderSignatureBuilder() override;
	virtual agpu::pipeline_builder_ptr createPipelineBuilder() override;
	virtual agpu::compute_pipeline_builder_ptr createComputePipelineBuilder() override;
	virtual agpu::command_allocator_ptr createCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & queue) override;
	virtual agpu::command_list_ptr createCommandList(agpu_command_list_type type, const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state) override;
	virtual agpu_shader_language getPreferredShaderLanguage() override;
	virtual agpu_shader_language getPreferredIntermediateShaderLanguage() override;
	virtual agpu_shader_language getPreferredHighLevelShaderLanguage() override;
	virtual agpu::framebuffer_ptr createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView) override;
	virtual agpu::renderpass_ptr createRenderPass(agpu_renderpass_description* description) override;
	virtual agpu::texture_ptr createTexture(agpu_texture_description* description) override;
	virtual agpu::fence_ptr createFence() override;
	virtual agpu_int getMultiSampleQualityLevels(agpu_uint sample_count) override;
	virtual agpu_bool hasTopLeftNdcOrigin() override;
	virtual agpu_bool hasBottomLeftTextureCoordinates() override;
	virtual agpu_bool isFeatureSupported(agpu_feature feature) override;
	virtual agpu::vr_system_ptr getVRSystem() override;
    virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;
    virtual agpu::state_tracker_cache_ptr createStateTrackerCache(const agpu::command_queue_ref & command_queue_family) override;

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
    VkPhysicalDeviceFeatures deviceFeatures;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    void *displayHandle;

    DECLARE_VK_EXTENSION_FP(GetDeviceProcAddr);

    // Debug layer extension pointers
    PFN_vkCreateDebugReportCallbackEXT fpCreateDebugReportCallbackEXT;
    PFN_vkDestroyDebugReportCallbackEXT fpDestroyDebugReportCallbackEXT;
    bool hasDebugReportExtension;
    VkDebugReportCallbackEXT debugReportCallback;

    // Required extension pointers.
    DECLARE_VK_EXTENSION_FP(GetPhysicalDeviceSurfaceSupportKHR);
    DECLARE_VK_EXTENSION_FP(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    DECLARE_VK_EXTENSION_FP(GetPhysicalDeviceSurfaceFormatsKHR);
    DECLARE_VK_EXTENSION_FP(GetPhysicalDeviceSurfacePresentModesKHR);
    DECLARE_VK_EXTENSION_FP(GetSwapchainImagesKHR);
    DECLARE_VK_EXTENSION_FP(CreateSwapchainKHR);
    DECLARE_VK_EXTENSION_FP(DestroySwapchainKHR);
    DECLARE_VK_EXTENSION_FP(AcquireNextImageKHR);
    DECLARE_VK_EXTENSION_FP(QueuePresentKHR);

    // VR support
    bool isVRDisplaySupported;
    bool isVRInputDevicesSupported;

    vr::IVRSystem *vrSystem;
    agpu::vr_system_ref vrSystemWrapper;

    // Queues
    std::vector<agpu::command_queue_ref> graphicsCommandQueues;
    std::vector<agpu::command_queue_ref> computeCommandQueues;
    std::vector<agpu::command_queue_ref> transferCommandQueues;

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

    bool clearImageWithColor(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask, VkClearColorValue *clearValue);
    bool clearImageWithDepthStencil(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask, VkClearDepthStencilValue *clearValue);
    bool copyBuffer(VkBuffer sourceBuffer, VkBuffer destBuffer, uint32_t regionCount, const VkBufferCopy *regions);
    bool copyBufferToImage(VkBuffer buffer, VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout destLayout, VkAccessFlags destAccessMask, uint32_t regionCount, const VkBufferImageCopy *regions);
    bool copyImageToBuffer(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout destLayout, VkAccessFlags destAccessMask, VkBuffer buffer, uint32_t regionCount, const VkBufferImageCopy *regions);

    bool setImageLayout(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask);
    VkImageMemoryBarrier barrierForImageLayoutTransition(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlags srcAccessMask, VkPipelineStageFlags &srcStages, VkPipelineStageFlags &dstStages);

private:
    bool checkDebugReportExtension();

    bool createSetupCommandBuffer();
    bool submitSetupCommandBuffer();

    std::mutex setupMutex;
    VkCommandPool setupCommandPool;
    VkCommandBuffer setupCommandBuffer;
    agpu::command_queue_ref setupQueue;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_DEVICE_HPP
