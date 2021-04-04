#ifndef _AGPU_D3D12_PLATFORM_H_
#define _AGPU_D3D12_PLATFORM_H_

#include "common.hpp"
#include "include_d3d12.hpp"
#include <vector>

namespace AgpuD3D12
{

struct Direct3D12AdapterDesc
{
	DXGI_ADAPTER_DESC1 desc;
	D3D12_FEATURE_DATA_ARCHITECTURE1 architecture;
	agpu_device_type deviceType;

	std::string name;

	agpu_bool isFeatureSupported(agpu_feature feature);
	agpu_uint getLimitValue(agpu_limit limit);

	bool fetchFromAdapterAndDevice(const ComPtr<IDXGIAdapter1>& adapter, const ComPtr<ID3D12Device>& device);
};

class Direct3D12Platform : public agpu::platform
{
public:
	Direct3D12Platform();
	~Direct3D12Platform();

	virtual agpu::device_ptr openDevice(agpu_device_open_info* openInfo) override;
	virtual agpu_cstring getName() override;
	virtual agpu_size getGpuCount() override;
	virtual agpu_cstring getGpuName(agpu_size gpu_index) override;
	virtual agpu_device_type getGpuDeviceType(agpu_size gpu_index) override;
	virtual agpu_bool isFeatureSupportedOnGPU(agpu_size gpu_index, agpu_feature feature) override;
	virtual agpu_uint getLimitValueOnGPU(agpu_size gpu_index, agpu_limit limit) override;
	virtual agpu_int getVersion() override;
	virtual agpu_int getImplementationVersion() override;
	virtual agpu_bool hasRealMultithreading() override;
	virtual agpu_bool isNative() override;
	virtual agpu_bool isCrossPlatform() override;
	virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;

	bool isSupported;
	std::vector<Direct3D12AdapterDesc> adapterDescs;

private:
	bool checkDirect3D12Implementation();
};

} // End of namespace AgpuD3D12

#endif //_AGPU_D3D12_PLATFORM_H_
