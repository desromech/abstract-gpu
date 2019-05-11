#include "platform.hpp"
#include "device.hpp"
#include "../Common/offline_shader_compiler.hpp"

namespace AgpuMetal
{

static agpu::platform_ref theMetalPlatform;

MetalPlatform::MetalPlatform()
{
}

MetalPlatform::~MetalPlatform()
{
}

agpu::device_ptr MetalPlatform::openDevice(agpu_device_open_info* openInfo)
{
    return AMtlDevice::open(openInfo).disown();
}

agpu_cstring MetalPlatform::getName()
{
    return "Metal";
}

agpu_int MetalPlatform::getVersion()
{
    return 10;
}

agpu_int MetalPlatform::getImplementationVersion()
{
    return 120;
}

agpu_bool MetalPlatform::hasRealMultithreading()
{
    return true;
}

agpu_bool MetalPlatform::isNative()
{
    return true;
}

agpu_bool MetalPlatform::isCrossPlatform()
{
    return false;
}

agpu::offline_shader_compiler_ptr MetalPlatform::createOfflineShaderCompiler()
{
    return AgpuCommon::GLSLangOfflineShaderCompiler::create().disown();
}

} // End of namespace AgpuMetal

AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    using namespace AgpuMetal;
    static std::once_flag platformCreatedFlag;
    std::call_once(platformCreatedFlag, []{
        theMetalPlatform = agpu::makeObject<MetalPlatform> ();
    });

    if (!platforms && numplatforms == 0)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 1;
        return AGPU_OK;
    }

    if(ret_numplatforms)
        *ret_numplatforms = 1;
    platforms[0] = reinterpret_cast<agpu_platform*> (theMetalPlatform.asPtrWithoutNewRef());
    return AGPU_OK;

}
