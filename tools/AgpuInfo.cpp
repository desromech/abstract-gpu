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
            printf("-- %02d: GPU: %s\n", int(gpuIndex), platform->getGpuName(gpuIndex));
            printf("--- deviceType: %s\n", deviceTypeToString(platform->getGpuDeviceType(gpuIndex)));
#define printFeature(featureName) printf("--- " #featureName ": %s\n", platform->isFeatureSupportedOnGPU(gpuIndex, featureName) ? "yes" : "no")
#define printLimit(limitName) printf("--- " #limitName ": %d\n", int(platform->getLimitValueOnGPU(gpuIndex, limitName)))

            printFeature(AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING);
            printFeature(AGPU_FEATURE_COHERENT_MEMORY_MAPPING);
            printFeature(AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING);
            printFeature(AGPU_FEATURE_COMMAND_LIST_REUSE);
            printFeature(AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE);

            printLimit(AGPU_LIMIT_NON_COHERENT_ATOM_SIZE);
            printLimit(AGPU_LIMIT_MIN_MEMORY_MAP_ALIGNMENT);
            printLimit(AGPU_LIMIT_MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT);
            printLimit(AGPU_LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT);
            printLimit(AGPU_LIMIT_MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT);
        }
    }

    return 0;
}
