#ifndef _AGPU_D3D12_PLATFORM_H_
#define _AGPU_D3D12_PLATFORM_H_

#include "common.hpp"
#include <mutex>

namespace AgpuD3D12
{

class Direct3D12Platform : public agpu::platform
{
public:
    Direct3D12Platform();
    ~Direct3D12Platform();

    virtual agpu::device_ptr openDevice(agpu_device_open_info* openInfo) override;
	virtual agpu_cstring getName() override;
    virtual agpu_size getGpuCount() override;
	virtual agpu_cstring getGpuName(agpu_size gpu_index) override;
	virtual agpu_int getVersion() override;
	virtual agpu_int getImplementationVersion() override;
	virtual agpu_bool hasRealMultithreading() override;
	virtual agpu_bool isNative() override;
	virtual agpu_bool isCrossPlatform() override;
    virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;

    bool isSupported;
    uint32_t gpuCount;
};

} // End of namespace Direct3D12Platform

#endif //_AGPU_D3D12_PLATFORM_H_
