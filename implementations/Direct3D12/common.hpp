#ifndef _AGPU_D3D12_COMMON_HPP
#define _AGPU_D3D12_COMMON_HPP

#include <AGPU/agpu.h>

#define CHECK_POINTER(pointer) if (!(pointer)) return AGPU_NULL_POINTER;
#define ERROR_IF_FAILED(op) if (FAILED(op)) return AGPU_ERROR;

void printError(const char *format, ...);

#endif //_AGPU_D3D12_COMMON_HPP
