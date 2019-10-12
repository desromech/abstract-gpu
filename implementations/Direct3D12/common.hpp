#ifndef _AGPU_D3D12_COMMON_HPP
#define _AGPU_D3D12_COMMON_HPP

#include <AGPU/agpu_impl.hpp>

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;
#define ERROR_IF_FAILED(op) if (FAILED(op)) return AGPU_ERROR;

namespace AgpuD3D12
{
#define deviceForDX device.as<ADXDevice> ()
#define lockWeakDeviceForDX weakDevice.lock().as<ADXDevice> ()

void printError(const char *format, ...);

} // End of namespace AgpuD3D12

#endif //_AGPU_D3D12_COMMON_HPP
