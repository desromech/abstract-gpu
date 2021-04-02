#include "platform.hpp"
#include "device.hpp"
#include "../Common/offline_shader_compiler.hpp"

namespace AgpuVulkan
{

static agpu::platform_ref theVulkanPlatform;

agpu_bool isFeatureSupportedOnGPU(agpu_feature feature, VkPhysicalDeviceProperties &deviceProperties, VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, VkPhysicalDeviceFeatures &deviceFeatures)
{
	switch (feature)
	{
	case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COMMAND_LIST_REUSE: return true;
	case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE: return false;
	default: return false;
	}
}

agpu_int getLimitValueOnGPU(agpu_limit limit, VkPhysicalDeviceProperties &deviceProperties, VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, VkPhysicalDeviceFeatures &deviceFeatures)
{
    switch(limit)
    {
    case AGPU_LIMIT_NON_COHERENT_ATOM_SIZE: return deviceProperties.limits.nonCoherentAtomSize;
    case AGPU_LIMIT_MIN_MEMORY_MAP_ALIGNMENT: return deviceProperties.limits.minMemoryMapAlignment;
    case AGPU_LIMIT_MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT: return deviceProperties.limits.minTexelBufferOffsetAlignment;
    case AGPU_LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT: return deviceProperties.limits.minUniformBufferOffsetAlignment;
    case AGPU_LIMIT_MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT: return deviceProperties.limits.minStorageBufferOffsetAlignment;
    default: return 0;
    }
}

VulkanPlatform::VulkanPlatform()
    : isSupported(0), gpuCount(0)
{
    isSupported = AVkDevice::checkVulkanImplementation(this);
}

VulkanPlatform::~VulkanPlatform()
{
}

agpu::device_ptr VulkanPlatform::openDevice(agpu_device_open_info* openInfo)
{
    return AVkDevice::open(openInfo).disown();
}

agpu_cstring VulkanPlatform::getName()
{
    return "Vulkan";
}

agpu_size VulkanPlatform::getGpuCount()
{
    return gpuCount;
}

agpu_cstring VulkanPlatform::getGpuName(agpu_size gpu_index)
{
    if(gpu_index < gpuCount)
        return deviceProperties[gpu_index].deviceName;
    return nullptr;
}

agpu_device_type VulkanPlatform::getGpuDeviceType(agpu_size gpu_index)
{
    if(gpu_index < gpuCount)
        return agpu_device_type(deviceProperties[gpu_index].deviceType);

    return AGPU_DEVICE_TYPE_OTHER;
}

agpu_bool VulkanPlatform::isFeatureSupportedOnGPU(agpu_size gpu_index, agpu_feature feature)
{
    if(gpu_index < gpuCount)
        return AgpuVulkan::isFeatureSupportedOnGPU(feature, deviceProperties[gpu_index], deviceMemoryProperties[gpu_index], deviceFeatures[gpu_index]);

    return false;
}

agpu_int VulkanPlatform::getLimitValueOnGPU(agpu_size gpu_index, agpu_limit limit)
{
    if(gpu_index < gpuCount)
        return AgpuVulkan::getLimitValueOnGPU(limit, deviceProperties[gpu_index], deviceMemoryProperties[gpu_index], deviceFeatures[gpu_index]);

    return 0;
}

agpu_int VulkanPlatform::getVersion()
{
    return 10;
}

agpu_int VulkanPlatform::getImplementationVersion()
{
    return 120;
}

agpu_bool VulkanPlatform::hasRealMultithreading()
{
    return true;
}

agpu_bool VulkanPlatform::isNative()
{
    return true;
}

agpu_bool VulkanPlatform::isCrossPlatform()
{
    return true;
}

agpu::offline_shader_compiler_ptr VulkanPlatform::createOfflineShaderCompiler()
{
    return AgpuCommon::GLSLangOfflineShaderCompiler::create().disown();
}

} // End of namespace AgpuVulkan

AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    using namespace AgpuVulkan;
    static std::once_flag platformCreatedFlag;
    std::call_once(platformCreatedFlag, []{
        theVulkanPlatform = agpu::makeObject<VulkanPlatform> ();
    });

    if(!theVulkanPlatform.as<VulkanPlatform> ()->isSupported)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 0;
        return AGPU_OK;
    }

    if (!platforms && numplatforms == 0)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 1;
        return AGPU_OK;
    }

    if(ret_numplatforms)
        *ret_numplatforms = 1;
    platforms[0] = reinterpret_cast<agpu_platform*> (theVulkanPlatform.asPtrWithoutNewRef());
    return AGPU_OK;

}
