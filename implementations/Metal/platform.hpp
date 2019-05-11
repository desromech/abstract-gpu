#ifndef AGPU_METAL_PLATFORM_HPP
#define AGPU_METAL_PLATFORM_HPP

#include "common.hpp"
#include <mutex>

namespace AgpuMetal
{

class MetalPlatform : public agpu::platform
{
public:
    MetalPlatform();
    ~MetalPlatform();

    virtual agpu::device_ptr openDevice(agpu_device_open_info* openInfo) override;
	virtual agpu_cstring getName() override;
	virtual agpu_int getVersion() override;
	virtual agpu_int getImplementationVersion() override;
	virtual agpu_bool hasRealMultithreading() override;
	virtual agpu_bool isNative() override;
	virtual agpu_bool isCrossPlatform() override;
    virtual agpu::offline_shader_compiler_ptr createOfflineShaderCompiler() override;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_PLATFORM_HPP
