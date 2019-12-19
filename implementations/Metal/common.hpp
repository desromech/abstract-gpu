#ifndef _AGPU_METAL_COMMON_HPP
#define _AGPU_METAL_COMMON_HPP

#include <AGPU/agpu_impl.hpp>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;

namespace AgpuMetal
{
#define deviceForMetal device.as<AgpuMetal::AMtlDevice> ()
#define lockWeakDeviceForMetal weakDevice.lock().as<AgpuMetal::AMtlDevice> ()

void printError(const char *format, ...);

inline const char *duplicateCString(const char *cstring)
{
    if(!cstring)
        return nullptr;

    auto resultSize = strlen(cstring);
    auto result = (char*)malloc(resultSize + 1);
    memcpy(result, cstring, resultSize);
    result[resultSize]  = 0;
    return result;
}

} // End of namespace AgpuMetal

#endif //_AGPU_METAL_COMMON_HPP
