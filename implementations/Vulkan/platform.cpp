#include "platform.hpp"
#include "device.hpp"

namespace AgpuVulkan
{

static agpu::platform_ref theVulkanPlatform;

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
