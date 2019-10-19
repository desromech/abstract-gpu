#ifndef AGPU_VULKAN_PLATFORM_HPP
#define AGPU_VULKAN_PLATFORM_HPP

#include "common.hpp"
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
	virtual agpu_int getVersion() override;
	virtual agpu_int getImplementationVersion() override;
	virtual agpu_bool hasRealMultithreading() override;
	virtual agpu_bool isNative() override;
	virtual agpu_bool isCrossPlatform() override;
    virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;

    bool isSupported;
    uint32_t gpuCount;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_PLATFORM_HPP
