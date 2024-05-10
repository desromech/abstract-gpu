#include "platform.hpp"
#include "device.hpp"
#include "../Common/offline_shader_compiler.hpp"

namespace AgpuVulkan
{

static agpu::platform_ref theVulkanPlatform;

agpu_bool isFeatureSupportedOnGPU(agpu_feature feature, VkPhysicalDeviceProperties &deviceProperties, VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, VkPhysicalDeviceFeatures &deviceFeatures)
{
	switch (feature)
	{
	case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COMMAND_LIST_REUSE: return true;
	case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE: return false;
	case AGPU_FEATURE_DUAL_SOURCE_BLENDING: return deviceFeatures.dualSrcBlend;
	case AGPU_FEATURE_GEOMETRY_SHADER: return deviceFeatures.geometryShader;
	case AGPU_FEATURE_TESSELLATION_SHADER: return deviceFeatures.tessellationShader;
	case AGPU_FEATURE_COMPUTE_SHADER: return true;
	case AGPU_FEATURE_MULTI_DRAW_INDIRECT: return deviceFeatures.multiDrawIndirect;
	case AGPU_FEATURE_DRAW_INDIRECT: return deviceFeatures.drawIndirectFirstInstance;
	case AGPU_FEATURE_TEXTURE_COMPRESSION_BC: return deviceFeatures.textureCompressionBC;
	case AGPU_FEATURE_TEXTURE_COMPRESSION_ETC2: return deviceFeatures.textureCompressionETC2;
	case AGPU_FEATURE_TEXTURE_COMPRESSION_ASTC_LDR: return deviceFeatures.textureCompressionASTC_LDR;
	case AGPU_FEATURE_SHADER_CLIP_DISTANCE: return deviceFeatures.shaderClipDistance;
	case AGPU_FEATURE_SHADER_CULL_DISTANCE: return deviceFeatures.shaderCullDistance;
	case AGPU_FEATURE_SHADER_FLOAT_64: return deviceFeatures.shaderFloat64;
	case AGPU_FEATURE_SHADER_INT_64: return deviceFeatures.shaderInt64;
	case AGPU_FEATURE_SHADER_INT_16: return deviceFeatures.shaderInt16;
	case AGPU_FEATURE_SAMPLE_SHADING: return deviceFeatures.sampleRateShading;
	case AGPU_FEATURE_FILL_MODE_NON_SOLID: return deviceFeatures.fillModeNonSolid;

	default: return false;
	}
}

agpu_uint getLimitValueOnGPU(agpu_limit limit, VkPhysicalDeviceProperties &deviceProperties, VkPhysicalDeviceMemoryProperties &deviceMemoryProperties, VkPhysicalDeviceFeatures &deviceFeatures)
{
    switch(limit)
    {
    case AGPU_LIMIT_NON_COHERENT_ATOM_SIZE: 			  			   return agpu_uint(deviceProperties.limits.nonCoherentAtomSize);
    case AGPU_LIMIT_MIN_MEMORY_MAP_ALIGNMENT: 			  			   return agpu_uint(deviceProperties.limits.minMemoryMapAlignment);
    case AGPU_LIMIT_MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT: 	  			   return agpu_uint(deviceProperties.limits.minTexelBufferOffsetAlignment);
    case AGPU_LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT:  			   return agpu_uint(deviceProperties.limits.minUniformBufferOffsetAlignment);
    case AGPU_LIMIT_MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT:  			   return agpu_uint(deviceProperties.limits.minStorageBufferOffsetAlignment);
	case AGPU_LIMIT_MIN_TEXTURE_DATA_OFFSET_ALIGNMENT: 	  			   return agpu_uint(deviceProperties.limits.optimalBufferCopyOffsetAlignment);
	case AGPU_LIMIT_MIN_TEXTURE_DATA_PITCH_ALIGNMENT: 	  			   return agpu_uint(deviceProperties.limits.optimalBufferCopyRowPitchAlignment);
	case AGPU_LIMIT_MAX_IMAGE_DIMENSION_1D: 			  			   return deviceProperties.limits.maxImageDimension1D;
	case AGPU_LIMIT_MAX_IMAGE_DIMENSION_2D: 			  			   return deviceProperties.limits.maxImageDimension2D;
	case AGPU_LIMIT_MAX_IMAGE_DIMENSION_3D: 			  			   return deviceProperties.limits.maxImageDimension3D;
	case AGPU_LIMIT_MAX_IMAGE_DIMENSION_CUBE: 			  			   return deviceProperties.limits.maxImageDimensionCube;
	case AGPU_LIMIT_MAX_IMAGE_ARRAY_LAYERS: 			  			   return deviceProperties.limits.maxImageArrayLayers;
	case AGPU_LIMIT_MAX_FRAMEBUFFER_WIDTH:  			  			   return deviceProperties.limits.maxFramebufferWidth;
	case AGPU_LIMIT_MAX_FRAMEBUFFER_HEIGHT: 			  			   return deviceProperties.limits.maxFramebufferHeight;
	case AGPU_LIMIT_MAX_FRAMEBUFFER_LAYERS: 			  			   return deviceProperties.limits.maxFramebufferLayers;
	case AGPU_LIMIT_MAX_CLIP_DISTANCES: 				  			   return deviceProperties.limits.maxClipDistances;
	case AGPU_LIMIT_MAX_CULL_DISTANCES: 				  			   return deviceProperties.limits.maxCullDistances;
	case AGPU_LIMIT_MAX_COMBINED_CLIP_AND_CULL_DISTANCES: 			   return deviceProperties.limits.maxCombinedClipAndCullDistances;
	case AGPU_LIMIT_MAX_TEXEL_BUFFER_ELEMENTS: 			  			   return deviceProperties.limits.maxTexelBufferElements;
	case AGPU_LIMIT_MAX_UNIFORM_BUFFER_RANGE:  			  			   return deviceProperties.limits.maxUniformBufferRange;
	case AGPU_LIMIT_MAX_STORAGE_BUFFER_RANGE:  			  			   return deviceProperties.limits.maxStorageBufferRange;
	case AGPU_LIMIT_MAX_PUSH_CONSTANTS_SIZE:   			  			   return deviceProperties.limits.maxPushConstantsSize;
	case AGPU_LIMIT_MAX_BOUND_SHADER_RESOURCE_BINDINGS:   			   return deviceProperties.limits.maxBoundDescriptorSets;
	case AGPU_LIMIT_MAX_COMPUTE_SHARED_MEMORY_SIZE: 	  			   return deviceProperties.limits.maxComputeSharedMemorySize;
	case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_INVOCATIONS:   			   return deviceProperties.limits.maxComputeWorkGroupInvocations;
	case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_XCOUNT: 		  			   return deviceProperties.limits.maxComputeWorkGroupCount[0];
	case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_XSIZE:  		  			   return deviceProperties.limits.maxComputeWorkGroupSize[0];
	case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_YCOUNT: 		  			   return deviceProperties.limits.maxComputeWorkGroupCount[1];
	case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_YSIZE:  		  			   return deviceProperties.limits.maxComputeWorkGroupSize[1];
	case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_ZCOUNT: 		  			   return deviceProperties.limits.maxComputeWorkGroupCount[2];
	case AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_ZSIZE:  		  			   return deviceProperties.limits.maxComputeWorkGroupSize[2];
	case AGPU_LIMIT_MAX_SAMPLER_LOD_BIAS: 				  			   return int(deviceProperties.limits.maxSamplerLodBias);
	case AGPU_LIMIT_MAX_SAMPLER_ANISOTROPY: 						   return int(deviceProperties.limits.maxSamplerAnisotropy);
	case AGPU_LIMIT_SAMPLED_IMAGE_COLOR_SUPPORTED_SAMPLE_COUNT_MASK:   return deviceProperties.limits.sampledImageColorSampleCounts;
	case AGPU_LIMIT_SAMPLED_IMAGE_INTEGER_SUPPORTED_SAMPLE_COUNT_MASK: return deviceProperties.limits.sampledImageIntegerSampleCounts;
	case AGPU_LIMIT_SAMPLED_IMAGE_DEPTH_SUPPORTED_SAMPLE_COUNT_MASK:   return deviceProperties.limits.sampledImageDepthSampleCounts;
	case AGPU_LIMIT_SAMPLED_IMAGE_STENCIL_SUPPORTED_SAMPLE_COUNT_MASK: return deviceProperties.limits.sampledImageStencilSampleCounts;
	case AGPU_LIMIT_STORAGE_IMAGE_SUPPORTED_SAMPLE_COUNT_MASK:   	   return deviceProperties.limits.storageImageSampleCounts;
	case AGPU_LIMIT_DEDICATED_VIDEO_MEMORY_IN_MB: {
		VkDeviceSize memory = 0;
		for(uint32_t i = 0; i < deviceMemoryProperties.memoryHeapCount; ++i)
		{
			auto &heap = deviceMemoryProperties.memoryHeaps[i];
			if(heap.flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				memory += heap.size;
		}

		return memory >> 20;
	}
	case AGPU_LIMIT_AVAILABLE_VIDEO_MEMORY_IN_MB: {
		VkDeviceSize memory = 0;
		for(uint32_t i = 0; i < deviceMemoryProperties.memoryHeapCount; ++i)
		{
			memory += deviceMemoryProperties.memoryHeaps[i].size;
		}

		return memory >> 20;
	}
    default: return 0;
    }
}

VulkanPlatform::VulkanPlatform()
    : isSupported(0), gpuCount(0)
{
    isSupported = AVkDevice::checkVulkanImplementation(this);
}

VulkanPlatform::~VulkanPlatform()
{
}

agpu::device_ptr VulkanPlatform::openDevice(agpu_device_open_info* openInfo)
{
    return AVkDevice::open(openInfo).disown();
}

agpu_cstring VulkanPlatform::getName()
{
    return "Vulkan";
}

agpu_size VulkanPlatform::getGpuCount()
{
    return gpuCount;
}

agpu_cstring VulkanPlatform::getGpuName(agpu_size gpu_index)
{
    if(gpu_index < gpuCount)
        return deviceProperties[gpu_index].deviceName;
    return nullptr;
}

agpu_device_type VulkanPlatform::getGpuDeviceType(agpu_size gpu_index)
{
    if(gpu_index < gpuCount)
        return agpu_device_type(deviceProperties[gpu_index].deviceType);

    return AGPU_DEVICE_TYPE_OTHER;
}

agpu_bool VulkanPlatform::isFeatureSupportedOnGPU(agpu_size gpu_index, agpu_feature feature)
{
    if(gpu_index < gpuCount)
        return AgpuVulkan::isFeatureSupportedOnGPU(feature, deviceProperties[gpu_index], deviceMemoryProperties[gpu_index], deviceFeatures[gpu_index]);

    return false;
}

agpu_uint VulkanPlatform::getLimitValueOnGPU(agpu_size gpu_index, agpu_limit limit)
{
    if(gpu_index < gpuCount)
        return AgpuVulkan::getLimitValueOnGPU(limit, deviceProperties[gpu_index], deviceMemoryProperties[gpu_index], deviceFeatures[gpu_index]);

    return 0;
}

agpu_int VulkanPlatform::getVersion()
{
    return 10;
}

agpu_int VulkanPlatform::getImplementationVersion()
{
    return 120;
}

agpu_bool VulkanPlatform::hasRealMultithreading()
{
    return true;
}

agpu_bool VulkanPlatform::isNative()
{
    return true;
}

agpu_bool VulkanPlatform::isCrossPlatform()
{
    return true;
}

agpu::offline_shader_compiler_ptr VulkanPlatform::createOfflineShaderCompiler()
{
    return AgpuCommon::GLSLangOfflineShaderCompiler::create().disown();
}

} // End of namespace AgpuVulkan

AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    using namespace AgpuVulkan;
    static std::once_flag platformCreatedFlag;
    std::call_once(platformCreatedFlag, []{
        theVulkanPlatform = agpu::makeObject<VulkanPlatform> ();
    });

    if(!theVulkanPlatform.as<VulkanPlatform> ()->isSupported)
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
    platforms[0] = reinterpret_cast<agpu_platform*> (theVulkanPlatform.asPtrWithoutNewRef());
    return AGPU_OK;

}
