#ifndef AGPU_VULKAN_PLATFORM_HPP
#define AGPU_VULKAN_PLATFORM_HPP

#include "common.hpp"
#include "include_vulkan.h"
#include <vector>
#include <string>
#include <mutex>

namespace AgpuVulkan
{

class VulkanPlatform : public agpu::platform
{
public:
    VulkanPlatform();
    ~VulkanPlatform();

    virtual agpu::device_ptr openDevice(agpu_device_open_info* openInfo) override;
	virtual agpu_cstring getName() override;
    virtual agpu_size getGpuCount() override;
	virtual agpu_cstring getGpuName(agpu_size gpu_index) override;
	virtual agpu_device_type getGpuDeviceType(agpu_size gpu_index) override;
	virtual agpu_bool isFeatureSupportedOnGPU(agpu_size gpu_index, agpu_feature feature)override;
	virtual agpu_uint getLimitValueOnGPU(agpu_size gpu_index, agpu_limit limit) override;
	virtual agpu_int getVersion() override;
	virtual agpu_int getImplementationVersion() override;
	virtual agpu_bool hasRealMultithreading() override;
	virtual agpu_bool isNative() override;
	virtual agpu_bool isCrossPlatform() override;
    virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;

    bool isSupported;
    uint32_t gpuCount;

    std::vector<VkPhysicalDeviceProperties> deviceProperties;
    std::vector<VkPhysicalDeviceMemoryProperties> deviceMemoryProperties;
    std::vector<VkPhysicalDeviceFeatures> deviceFeatures;
};

agpu_bool isFeatureSupportedOnGPU(agpu_feature feature, VkPhysicalDeviceProperties &deviceProperties, VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, VkPhysicalDeviceFeatures &deviceFeatures);
agpu_uint getLimitValueOnGPU(agpu_limit limit, VkPhysicalDeviceProperties &deviceProperties, VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, VkPhysicalDeviceFeatures &deviceFeatures);


} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_PLATFORM_HPP
