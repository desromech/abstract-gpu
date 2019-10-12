#include "platform.hpp"
#include "device.hpp"
#include "../Common/offline_shader_compiler.hpp"

namespace AgpuD3D12
{
static agpu::platform_ref theDirect3D12Platform;

Direct3D12Platform::Direct3D12Platform()
    : isSupported(0), gpuCount(0)
{
    isSupported = ADXDevice::checkDirect3D12Implementation(this);
}

Direct3D12Platform::~Direct3D12Platform()
{
}

agpu::device_ptr Direct3D12Platform::openDevice(agpu_device_open_info* openInfo)
{
    return ADXDevice::open(openInfo).disown();
}

agpu_cstring Direct3D12Platform::getName()
{
    return "Direct3D12";
}

agpu_int Direct3D12Platform::getVersion()
{
    return 120;
}

agpu_int Direct3D12Platform::getImplementationVersion()
{
    return 120;
}

agpu_bool Direct3D12Platform::hasRealMultithreading()
{
    return true;
}

agpu_bool Direct3D12Platform::isNative()
{
    return true;
}

agpu_bool Direct3D12Platform::isCrossPlatform()
{
    return false;
}

agpu::offline_shader_compiler_ptr Direct3D12Platform::createOfflineShaderCompiler()
{
    return AgpuCommon::GLSLangOfflineShaderCompiler::create().disown();
}

} // End of namespace AgpuD3D12


AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    using namespace AgpuD3D12;
    static std::once_flag platformCreatedFlag;
    std::call_once(platformCreatedFlag, []{
        theDirect3D12Platform = agpu::makeObject<Direct3D12Platform> ();
    });

    if(!theDirect3D12Platform.as<Direct3D12Platform> ()->isSupported)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 0;
        return AGPU_OK;
    }

    if (!platforms && numplatforms == 0)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 1;
        return AGPU_OK;
    }

    if(ret_numplatforms)
        *ret_numplatforms = 1;
    platforms[0] = reinterpret_cast<agpu_platform*> (theDirect3D12Platform.asPtrWithoutNewRef());
    return AGPU_OK;

}
