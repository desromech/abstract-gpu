#include "platform.hpp"
#include "device.hpp"

_agpu_platform theD3D12Platform = { &agpu_d3d12_icd_dispatch };

AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    if (numplatforms < 1)
        return AGPU_INVALID_OPERATION;

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

