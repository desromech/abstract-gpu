#ifndef AGPU_METAL_PLATFORM_HPP
#define AGPU_METAL_PLATFORM_HPP

#include "common.hpp"
#import <Metal/Metal.h>
#include <vector>
#include <string>

namespace AgpuMetal
{

class MetalPlatform : public agpu::platform
{
public:
    MetalPlatform();
    ~MetalPlatform();

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
    
    NSArray<id<MTLDevice>> *allDevices;
};

agpu_bool isFeatureSupportedInDevice(id<MTLDevice> device, agpu_feature feature);
agpu_uint getLimitValueInDevice(id<MTLDevice> device, agpu_limit limit);
agpu_device_type getDeviceType(id<MTLDevice> device);

} // End of namespace AgpuMetal

#endif //AGPU_METAL_PLATFORM_HPP
