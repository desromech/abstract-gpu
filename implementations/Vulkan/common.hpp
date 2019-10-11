#ifndef _AGPU_VULKAN_COMMON_HPP
#define _AGPU_VULKAN_COMMON_HPP

#include <AGPU/agpu_impl.hpp>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef _MSC_VER
#pragma warning(disable : 4146)
#endif

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;
#define CONVERT_VULKAN_ERROR(error) if(error) return AGPU_ERROR;

namespace AgpuVulkan
{
#define deviceForVk device.as<AVkDevice> ()
#define lockWeakDeviceForVK weakDevice.lock().as<AVkDevice> ()

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

} // End of namespace AgpuVulkan


#endif //_AGPU_VULKAN_COMMON_HPP
