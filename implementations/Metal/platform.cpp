#include "platform.hpp"

_agpu_platform theMetalPlatform = { &agpu_metal_icd_dispatch };

AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    if (!platforms && numplatforms == 0)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 1;
        return AGPU_OK;
    }

    if (ret_numplatforms)
        *ret_numplatforms = 1;
    platforms[0] = &theMetalPlatform;
    return AGPU_OK;
}

AGPU_EXPORT agpu_device* agpuOpenDevice ( agpu_platform* platform, agpu_device_open_info* openInfo )
{
    return nullptr;
}

AGPU_EXPORT agpu_cstring agpuGetPlatformName ( agpu_platform* platform )
{
    if(platform != &theMetalPlatform)
        return nullptr;
    return "Metal";
}

AGPU_EXPORT agpu_int agpuGetPlatformVersion ( agpu_platform* platform )
{
    if(platform != &theMetalPlatform)
        return 0;
    return 10;
}

AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion ( agpu_platform* platform )
{
    if(platform != &theMetalPlatform)
        return 0;
    return 1;
}

AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading ( agpu_platform* platform )
{
    return true;
}

AGPU_EXPORT agpu_bool agpuIsNativePlatform ( agpu_platform* platform )
{
    return true;
}

AGPU_EXPORT agpu_bool agpuIsCrossPlatform ( agpu_platform* platform )
{
    return false;
}
