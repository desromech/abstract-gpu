#ifndef _AGPU_VULKAN_COMMON_HPP
#define _AGPU_VULKAN_COMMON_HPP

#include <AGPU/agpu.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#pragma warning(disable : 4146)
#endif

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;
#define MAKE_CURRENT() if (!makeCurrent) return AGPU_NOT_CURRENT_CONTEXT;
#define CHECK_CURRENT() if (!isCurrentContext()) return AGPU_NOT_CURRENT_CONTEXT;
#define CONVERT_VULKAN_ERROR(error) if(error) return AGPU_ERROR;

void printError(const char *format, ...);

inline size_t alignedTo(size_t value, size_t alignment)
{
    return (value + alignment - 1) & (-alignment);
}

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

#endif //_AGPU_VULKAN_COMMON_HPP
