#include "platform.hpp"
#include "device.hpp"

_agpu_platform theD3D12Platform = { &agpu_d3d12_icd_dispatch };

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
    platforms[0] = &theD3D12Platform;
    return AGPU_OK;
}

AGPU_EXPORT agpu_device* agpuOpenDevice(agpu_platform* platform, agpu_device_open_info* openInfo)
{
    if (platform != &theD3D12Platform)
        return nullptr;

    return agpu_device::open(openInfo);
}

AGPU_EXPORT agpu_cstring agpuGetPlatformName(agpu_platform* platform)
{
    if (platform != &theD3D12Platform)
        return nullptr;

    return "Direct3D 12";
}

AGPU_EXPORT agpu_int agpuGetPlatformVersion(agpu_platform* platform)
{
    if (platform != &theD3D12Platform)
        return 0;

    return 10;
}

AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion(agpu_platform* platform)
{
    if (platform != &theD3D12Platform)
        return 0;

    return 120;
}

AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading(agpu_platform* platform)
{
    return platform == &theD3D12Platform;
}

AGPU_EXPORT agpu_bool agpuIsNativePlatform(agpu_platform* platform)
{
    return platform == &theD3D12Platform;
}

AGPU_EXPORT agpu_bool agpuIsCrossPlatform(agpu_platform* platform)
{
    return platform != &theD3D12Platform;
}
