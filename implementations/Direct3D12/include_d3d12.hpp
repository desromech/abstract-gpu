#ifndef AGPU_D3D12_INCLUDE_D3D12_HPP
#define AGPU_D3D12_INCLUDE_D3D12_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

namespace AgpuD3D12
{

	using Microsoft::WRL::ComPtr;

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_INCLUDE_D3D12_HPP