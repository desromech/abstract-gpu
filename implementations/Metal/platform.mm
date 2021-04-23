#include "platform.hpp"
#include "device.hpp"
#include "../Common/offline_shader_compiler.hpp"
#include <mutex>

namespace AgpuMetal
{

static agpu::platform_ref theMetalPlatform;

agpu_bool isFeatureSupportedInDevice(id<MTLDevice> device, agpu_feature feature)
{
    switch(feature)
    {
    case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING:
    case AGPU_FEATURE_COHERENT_MEMORY_MAPPING:
    case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING:
        return true;

    case AGPU_FEATURE_COMMAND_LIST_REUSE:
    case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE:
        return false;

    case AGPU_FEATURE_DUAL_SOURCE_BLENDING:
	case AGPU_FEATURE_COMPUTE_SHADER:
	case AGPU_FEATURE_TEXTURE_COMPRESSION_BC:
    case AGPU_FEATURE_SHADER_CLIP_DISTANCE:
	case AGPU_FEATURE_SHADER_CULL_DISTANCE:
    case AGPU_FEATURE_SAMPLE_SHADING:
    case AGPU_FEATURE_SHADER_INT_16:
        return true;

    case AGPU_FEATURE_GEOMETRY_SHADER:
	case AGPU_FEATURE_TESSELLATION_SHADER:
	case AGPU_FEATURE_MULTI_DRAW_INDIRECT:
	case AGPU_FEATURE_DRAW_INDIRECT:
	case AGPU_FEATURE_TEXTURE_COMPRESSION_ETC2:
	case AGPU_FEATURE_TEXTURE_COMPRESSION_ASTC_LDR:
	case AGPU_FEATURE_SHADER_FLOAT_64:
	case AGPU_FEATURE_SHADER_INT_64:
	case AGPU_FEATURE_FILL_MODE_NON_SOLID:
        return false;

    default:
        return false;
    }
}

agpu_uint getLimitValueInDevice(id<MTLDevice> device, agpu_limit limit)
{
    // See for several of these limits: https://developer.apple.com/metal/Metal-Feature-Set-Tables.pdf
    switch(limit)
    {
    case AGPU_LIMIT_NON_COHERENT_ATOM_SIZE: return 256;
    case AGPU_LIMIT_MIN_MEMORY_MAP_ALIGNMENT: return 256;
    case AGPU_LIMIT_MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT: return 256;
    case AGPU_LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT: return 256;
    case AGPU_LIMIT_MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT: return 256;
	case AGPU_LIMIT_MIN_TEXTURE_DATA_OFFSET_ALIGNMENT: return 256;
	case AGPU_LIMIT_MIN_TEXTURE_DATA_PITCH_ALIGNMENT: return 256;

    case AGPU_LIMIT_MAX_IMAGE_DIMENSION_1D: return 16384;
    case AGPU_LIMIT_MAX_IMAGE_DIMENSION_2D:
    case AGPU_LIMIT_MAX_FRAMEBUFFER_WIDTH:
    case AGPU_LIMIT_MAX_FRAMEBUFFER_HEIGHT:
        return 16384;
    case AGPU_LIMIT_MAX_IMAGE_DIMENSION_3D: return 16384;
    case AGPU_LIMIT_MAX_IMAGE_DIMENSION_CUBE: return 2048;
	case AGPU_LIMIT_MAX_IMAGE_ARRAY_LAYERS:
    case AGPU_LIMIT_MAX_FRAMEBUFFER_LAYERS:
        return 2048;

    case AGPU_LIMIT_MAX_SAMPLER_ANISOTROPY: return 16;

    case AGPU_LIMIT_SAMPLED_IMAGE_COLOR_SUPPORTED_SAMPLE_COUNT_MASK:
	case AGPU_LIMIT_SAMPLED_IMAGE_INTEGER_SUPPORTED_SAMPLE_COUNT_MASK:
	case AGPU_LIMIT_SAMPLED_IMAGE_DEPTH_SUPPORTED_SAMPLE_COUNT_MASK:
	case AGPU_LIMIT_SAMPLED_IMAGE_STENCIL_SUPPORTED_SAMPLE_COUNT_MASK:
	case AGPU_LIMIT_STORAGE_IMAGE_SUPPORTED_SAMPLE_COUNT_MASK:
    {
        agpu_uint result = 0;
        if([device supportsTextureSampleCount: 1])
            result |= (1<<0);
        if([device supportsTextureSampleCount: 2])
            result |= (1<<1);
        if([device supportsTextureSampleCount: 4])
            result |= (1<<2);
        if([device supportsTextureSampleCount: 8])
            result |= (1<<3);
        if([device supportsTextureSampleCount: 16])
            result |= (1<<4);
        if([device supportsTextureSampleCount: 32])
            result |= (1<<5);
        return result;
    }

    case AGPU_LIMIT_MAX_COMPUTE_SHARED_MEMORY_SIZE: return device.maxThreadgroupMemoryLength;
    case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_XSIZE: return device.maxThreadsPerThreadgroup.width;
    case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_YSIZE: return device.maxThreadsPerThreadgroup.height;
    case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_ZSIZE: return device.maxThreadsPerThreadgroup.depth;

    case AGPU_LIMIT_AVAILABLE_VIDEO_MEMORY_IN_MB: return agpu_uint(device.recommendedMaxWorkingSetSize >> 20);
    default: return 0;
    }
}

agpu_device_type getDeviceType(id<MTLDevice> device)
{
    if (@available(macos 10.15, *))
    {
        return device.hasUnifiedMemory ? AGPU_DEVICE_TYPE_INTEGRATED_GPU : AGPU_DEVICE_TYPE_DISCRETE_GPU;
    }

    if([device.name rangeOfString: @"amd" options: NSCaseInsensitiveSearch].location != NSNotFound ||
    [device.name rangeOfString: @"nvidia" options: NSCaseInsensitiveSearch].location != NSNotFound
    )
    {
        return AGPU_DEVICE_TYPE_DISCRETE_GPU;
    }

    return AGPU_DEVICE_TYPE_INTEGRATED_GPU;
}

MetalPlatform::MetalPlatform()
    : allDevices(nil)
{
    allDevices = MTLCopyAllDevices();
}

MetalPlatform::~MetalPlatform()
{
    if(allDevices)
        [allDevices release];
}

agpu::device_ptr MetalPlatform::openDevice(agpu_device_open_info* openInfo)
{
    auto gpuIndex = openInfo->gpu_index;
    if(gpuIndex < 0)
        gpuIndex = 0;
    if(size_t(gpuIndex) >= allDevices.count)
        return nullptr;

    return AMtlDevice::open(allDevices[gpuIndex], openInfo).disown();
}

agpu_cstring MetalPlatform::getName()
{
    return "Metal";
}

agpu_size MetalPlatform::getGpuCount()
{
    return allDevices.count;
}

agpu_cstring MetalPlatform::getGpuName(agpu_size gpu_index)
{
    if(gpu_index < allDevices.count)
        return [allDevices[gpu_index].name UTF8String];
    return "Default";
}

agpu_device_type MetalPlatform::getGpuDeviceType(agpu_size gpu_index)
{
    if(gpu_index < allDevices.count)
    {
        return getDeviceType(allDevices[gpu_index]);
    }

    return AGPU_DEVICE_TYPE_OTHER;
}

agpu_bool MetalPlatform::isFeatureSupportedOnGPU(agpu_size gpu_index, agpu_feature feature)
{
    if(gpu_index < allDevices.count)
    {
        auto device = allDevices[gpu_index];
        auto result = isFeatureSupportedInDevice(device, feature);
        return result;
    }
    return false;
}

agpu_uint MetalPlatform::getLimitValueOnGPU(agpu_size gpu_index, agpu_limit limit)
{
    if(gpu_index < allDevices.count)
    {
        auto device = allDevices[gpu_index];
        auto result = getLimitValueInDevice(device, limit);
        return result;
    }

    return 0;
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

    // We need at least a single Metal device to work.
    auto mtlPlatform = theMetalPlatform.as<MetalPlatform> ();
    if(!mtlPlatform->allDevices || mtlPlatform->allDevices.count == 0)
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
    platforms[0] = reinterpret_cast<agpu_platform*> (theMetalPlatform.asPtrWithoutNewRef());
    return AGPU_OK;

}
