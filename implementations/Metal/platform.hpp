#ifndef AGPU_METAL_PLATFORM_HPP
#define AGPU_METAL_PLATFORM_HPP

#include "object.hpp"

struct _agpu_platform
{
    agpu_icd_dispatch *dispatch;
};

extern _agpu_platform theMetalPlatform;

#endif //AGPU_METAL_PLATFORM_HPP
