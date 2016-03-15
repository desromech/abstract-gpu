#include "platform.hpp"
#include "device.hpp"

_agpu_platform theGLPlatform = {&agpu_gl_icd_dispatch};

AGPU_EXPORT agpu_error agpuGetPlatforms ( agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms )
{
    if(numplatforms < 1)
        return AGPU_INVALID_OPERATION;

    if(ret_numplatforms)
        *ret_numplatforms = 1;
    platforms[0] = &theGLPlatform;
    return AGPU_OK;
}

AGPU_EXPORT agpu_device* agpuOpenDevice ( agpu_platform* platform, agpu_device_open_info* openInfo )
{
    if(platform != &theGLPlatform)
        return nullptr;

    return agpu_device::open(openInfo);
}

AGPU_EXPORT agpu_cstring agpuGetPlatformName(agpu_platform* platform)
{
    if (platform != &theGLPlatform)
        return nullptr;

    return "OpenGL 4.x Core";
}

AGPU_EXPORT agpu_int agpuGetPlatformVersion(agpu_platform* platform)
{
    if (platform != &theGLPlatform)
        return -1;

    return 100;
}

AGPU_EXPORT agpu_int agpuGetPlatformImplementationVersion(agpu_platform* platform)
{
    if (platform != &theGLPlatform)
        return -1;

    return 1;
}

AGPU_EXPORT agpu_bool agpuPlatformHasRealMultithreading(agpu_platform* platform)
{
    return false;
}

AGPU_EXPORT agpu_bool agpuIsNativePlatform(agpu_platform* platform)
{
    return false;
}

AGPU_EXPORT agpu_bool agpuIsCrossPlatform(agpu_platform* platform)
{
    return platform == &theGLPlatform;
}
