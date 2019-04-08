#ifndef _AGPU_GL_COMMON_HPP
#define _AGPU_GL_COMMON_HPP

#include <AGPU/agpu_impl.hpp>
#include <stdarg.h>
#include <stdio.h>

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;
#define MAKE_CURRENT() if (!makeCurrent) return AGPU_NOT_CURRENT_CONTEXT;
#define CHECK_CURRENT() if (!isCurrentContext()) return AGPU_NOT_CURRENT_CONTEXT;

namespace AgpuGL
{
#define deviceForGL device.as<GLDevice> ()
#define lockWeakDeviceForGL weakDevice.lock().as<GLDevice> ()

void printError(const char *format, ...);
} // End of namespace


#endif //_AGPU_GL_COMMON_HPP
