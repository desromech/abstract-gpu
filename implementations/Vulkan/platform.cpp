#include "platform.hpp"
#include "device.hpp"

_agpu_platform theVulkanPlatform = { &agpu_vulkan_icd_dispatch };

AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    if (numplatforms < 1)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 1;
        return AGPU_OK;
    }

    if (ret_numplatforms)
        *ret_numplatforms = 1;
    platforms[0] = &theVulkanPlatform;
    return AGPU_OK;
}

AGPU_EXPORT agpu_device* agpuOpenDevice(agpu_platform* platform, agpu_device_open_info* openInfo)
{
    if (platform != &theVulkanPlatform)
        return nullptr;

    return agpu_device::open(openInfo);
}

AGPU_EXPORT agpu_cstring agpuGetPlatformName(agpu_platform* platform)
{
    if (platform != &theVulkanPlatform)
        return nullptr;

    return "Vulkan";
}

AGPU_EXPORT agpu_int agpuGetPlatformVersion(agpu_platform* platform)
{
    if (platform != &theVulkanPlatform)
        return 0;

    return 10;
}

AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion(agpu_platform* platform)
{
    if (platform != &theVulkanPlatform)
        return 0;

    return 120;
}

AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading(agpu_platform* platform)
{
    return platform == &theVulkanPlatform;
}

AGPU_EXPORT agpu_bool agpuIsNativePlatform(agpu_platform* platform)
{
    return platform == &theVulkanPlatform;
}

AGPU_EXPORT agpu_bool agpuIsCrossPlatform(agpu_platform* platform)
{
    return platform == &theVulkanPlatform;
}
