#include <vector>

#include <stdio.h>
#include <AGPU/agpu.hpp>

const char *deviceTypeToString(agpu_device_type type)
{
    switch(type)
    {
    case AGPU_DEVICE_TYPE_OTHER: return "Other";
    case AGPU_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
    case AGPU_DEVICE_TYPE_DISCRETE_GPU: return "Discrete GPU";
    case AGPU_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual GPU";
    case AGPU_DEVICE_TYPE_CPU: return "CPU";
    default: return "Unknown";
    }
}

int main()
{
    agpu_size platformCount;
    agpuGetPlatforms(0, nullptr, &platformCount);

    if(platformCount == 0)
    {
        printf("No AGPU platforms are available.\n");
        return 0;
    }

    // Get the platform.
    std::vector<agpu_platform*> platforms(platformCount);
    agpuGetPlatforms(platformCount, &platforms[0], &platformCount);

    // List the platform names, and their GPUs.
    for(size_t i = 0; i < platforms.size(); ++i)
    {
        auto platform = platforms[i];
        printf("- %02d: Platform: %s\n", int(i), platform->getName());

        auto gpuCount = platform->getGpuCount();
        for(agpu_size gpuIndex = 0; gpuIndex < gpuCount; ++gpuIndex)
        {
            printf("-- %02d: %s: %s\n", int(gpuIndex), deviceTypeToString(platform->getGpuDeviceType(gpuIndex)), platform->getGpuName(gpuIndex));
#define printFeature(featureName) printf("--- " #featureName ": %s\n", platform->isFeatureSupportedOnGPU(gpuIndex, featureName) ? "yes" : "no")
#define printLimit(limitName) printf("--- " #limitName ": %u\n", int(platform->getLimitValueOnGPU(gpuIndex, limitName)))

            printFeature(AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING);
            printFeature(AGPU_FEATURE_COHERENT_MEMORY_MAPPING);
            printFeature(AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING);
            printFeature(AGPU_FEATURE_COMMAND_LIST_REUSE);
            printFeature(AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE);
            printFeature(AGPU_FEATURE_DUAL_SOURCE_BLENDING);
            printFeature(AGPU_FEATURE_GEOMETRY_SHADER);
        	printFeature(AGPU_FEATURE_TESSELLATION_SHADER);
        	printFeature(AGPU_FEATURE_COMPUTE_SHADER);
        	printFeature(AGPU_FEATURE_MULTI_DRAW_INDIRECT);
        	printFeature(AGPU_FEATURE_DRAW_INDIRECT);
        	printFeature(AGPU_FEATURE_TEXTURE_COMPRESSION_BC);
        	printFeature(AGPU_FEATURE_TEXTURE_COMPRESSION_ETC2);
        	printFeature(AGPU_FEATURE_TEXTURE_COMPRESSION_ASTC_LDR);
        	printFeature(AGPU_FEATURE_SHADER_CLIP_DISTANCE);
        	printFeature(AGPU_FEATURE_SHADER_CULL_DISTANCE);
        	printFeature(AGPU_FEATURE_SHADER_FLOAT_64);
        	printFeature(AGPU_FEATURE_SHADER_INT_64);
        	printFeature(AGPU_FEATURE_SHADER_INT_16);
        	printFeature(AGPU_FEATURE_SAMPLE_SHADING);
        	printFeature(AGPU_FEATURE_FILL_MODE_NON_SOLID);

            printLimit(AGPU_LIMIT_NON_COHERENT_ATOM_SIZE);
            printLimit(AGPU_LIMIT_MIN_MEMORY_MAP_ALIGNMENT);
            printLimit(AGPU_LIMIT_MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT);
            printLimit(AGPU_LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT);
            printLimit(AGPU_LIMIT_MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT);
        	printLimit(AGPU_LIMIT_MAX_IMAGE_DIMENSION_1D);
        	printLimit(AGPU_LIMIT_MAX_IMAGE_DIMENSION_2D);
        	printLimit(AGPU_LIMIT_MAX_IMAGE_DIMENSION_3D);
        	printLimit(AGPU_LIMIT_MAX_IMAGE_DIMENSION_CUBE);
        	printLimit(AGPU_LIMIT_MAX_IMAGE_ARRAY_LAYERS);
        	printLimit(AGPU_LIMIT_MAX_FRAMEBUFFER_WIDTH);
        	printLimit(AGPU_LIMIT_MAX_FRAMEBUFFER_HEIGHT);
        	printLimit(AGPU_LIMIT_MAX_FRAMEBUFFER_LAYERS);
        	printLimit(AGPU_LIMIT_MAX_CLIP_DISTANCES);
        	printLimit(AGPU_LIMIT_MAX_CULL_DISTANCES);
        	printLimit(AGPU_LIMIT_MAX_COMBINED_CLIP_AND_CULL_DISTANCES);
        	printLimit(AGPU_LIMIT_MAX_TEXEL_BUFFER_ELEMENTS);
        	printLimit(AGPU_LIMIT_MAX_UNIFORM_BUFFER_RANGE);
        	printLimit(AGPU_LIMIT_MAX_STORAGE_BUFFER_RANGE);
        	printLimit(AGPU_LIMIT_MAX_PUSH_CONSTANTS_SIZE);
        	printLimit(AGPU_LIMIT_MAX_BOUND_SHADER_RESOURCE_BINDINGS);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_SHARED_MEMORY_SIZE);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_INVOCATIONS);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_XCOUNT);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_XSIZE);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_YCOUNT);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_YSIZE);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_ZCOUNT);
        	printLimit(AGPU_LIMIT_MAX_COMPUTE_WORK_GROUP_ZSIZE);
        	printLimit(AGPU_LIMIT_MAX_SAMPLER_LOD_BIAS);
        	printLimit(AGPU_LIMIT_MAX_SAMPLER_ANISOTROPY);
        	printLimit(AGPU_LIMIT_SAMPLED_IMAGE_COLOR_SUPPORTED_SAMPLE_COUNT_MASK);
        	printLimit(AGPU_LIMIT_SAMPLED_IMAGE_INTEGER_SUPPORTED_SAMPLE_COUNT_MASK);
        	printLimit(AGPU_LIMIT_SAMPLED_IMAGE_DEPTH_SUPPORTED_SAMPLE_COUNT_MASK);
        	printLimit(AGPU_LIMIT_SAMPLED_IMAGE_STENCIL_SUPPORTED_SAMPLE_COUNT_MASK);
        	printLimit(AGPU_LIMIT_STORAGE_IMAGE_SUPPORTED_SAMPLE_COUNT_MASK);
        	printLimit(AGPU_LIMIT_DEDICATED_VIDEO_MEMORY_IN_MB);
        	printLimit(AGPU_LIMIT_AVAILABLE_VIDEO_MEMORY_IN_MB);
        }
    }

    return 0;
}
