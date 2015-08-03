#ifndef _AGPU_D3D12_PLATFORM_H_
#define _AGPU_D3D12_PLATFORM_H_

#include "object.hpp"

struct _agpu_platform
{
    agpu_icd_dispatch *dispatch;
};

extern _agpu_platform theD3D12Platform;

#endif //_AGPU_D3D12_PLATFORM_H_
