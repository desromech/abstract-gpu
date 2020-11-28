#ifndef _AGPU_D3D12_COMMON_HPP
#define _AGPU_D3D12_COMMON_HPP

#include <AGPU/agpu_impl.hpp>
#include <winerror.h>

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;
#define ERROR_IF_FAILED(op) do { \
	auto hresult = op; \
	if(FAILED(hresult)) return AgpuD3D12::convertErrorCode(hresult); \
} while(0)

namespace AgpuD3D12
{
#define deviceForDX device.as<ADXDevice> ()
#define lockWeakDeviceForDX weakDevice.lock().as<ADXDevice> ()

void printError(const char *format, ...);
agpu_error convertErrorCode(HRESULT errorCode);

} // End of namespace AgpuD3D12

#endif //_AGPU_D3D12_COMMON_HPP
