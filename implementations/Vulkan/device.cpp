#include <algorithm>
#include "device.hpp"
#include "command_queue.hpp"
#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "renderpass.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "pipeline_builder.hpp"
#include "compute_pipeline_builder.hpp"
#include "platform.hpp"
#include "shader_signature_builder.hpp"
#include "shader.hpp"
#include "vertex_layout.hpp"
#include "vertex_binding.hpp"
#include "buffer.hpp"
#include "fence.hpp"
#include "vr_system.hpp"
#include "sampler.hpp"
#include "../Common/offline_shader_compiler.hpp"
#include "../Common/state_tracker_cache.hpp"

#define GET_INSTANCE_PROC_ADDR(procName) \
    {                                                                          \
        fp##procName = (PFN_vk##procName)vkGetInstanceProcAddr(vulkanInstance, "vk" #procName); \
        if (fp##procName == NULL) {                                    \
            printError("vkGetInstanceProcAddr failed to find vk" #procName, "vkGetInstanceProcAddr Failure\n");                         \
            return false; \
        }                                                                      \
    }

#define GET_DEVICE_PROC_ADDR(procName) \
    {                                                                          \
        fp##procName = (PFN_vk##procName)fpGetDeviceProcAddr(device, "vk" #procName); \
        if (fp##procName == NULL) {                                    \
            printError("vkGetDeviceProcAddr failed to find vk" #procName, "vkGetInstanceProcAddr Failure\n");                         \
            return false; \
        }                                                                      \
    }

namespace AgpuVulkan
{

void printError(const char *format, ...)
{
    char buffer[2048];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 2048, format, args);
#ifdef _WIN32
    OutputDebugStringA(buffer);
#endif
    fputs(buffer, stderr);
    va_end(args);
}

const char *validationLayerNames[] = {
    "VK_LAYER_LUNARG_standard_validation",
};

constexpr size_t validationLayerCount = sizeof(validationLayerNames) / sizeof(validationLayerNames[0]);

const char *coreRequiredInstanceExtensionNames[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(_WIN32)
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif defined(__unix__)
    VK_KHR_XCB_SURFACE_EXTENSION_NAME
#else
#error unsupported platform
#endif
};

constexpr size_t coreRequiredInstanceExtensionCount = sizeof(coreRequiredInstanceExtensionNames) / sizeof(coreRequiredInstanceExtensionNames[0]);

const char *coreRequiredDeviceExtensionNames[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

constexpr size_t coreRequiredDeviceExtensionCount = sizeof(coreRequiredDeviceExtensionNames) / sizeof(coreRequiredDeviceExtensionNames[0]);


static bool hasLayer(const std::string &layerName, const std::vector<VkLayerProperties> &layerProperties)
{
    for (auto &layer : layerProperties)
    {
        if (layer.layerName == layerName)
            return true;
    }

    return false;
}

static bool hasExtension(const std::string &extensionName, const std::vector<VkExtensionProperties> &extensionProperties)
{
    for (auto &extension : extensionProperties)
    {
        if (extension.extensionName == extensionName)
            return true;
    }

    return false;
}

static bool hasValidationLayers(const std::vector<VkLayerProperties> &layers)
{
    for (size_t i = 0; i < validationLayerCount; ++i)
    {
        if (!hasLayer(validationLayerNames[i], layers))
            return false;
    }

    return true;
}

static bool hasRequiredExtensions(const std::vector<VkExtensionProperties> &extensions, const std::vector<std::string> &requiredExtensionNames)
{
    for(auto &requiredExtensionName : requiredExtensionNames)
    {
        if (!hasExtension(requiredExtensionName, extensions))
            return false;
    }

    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackFunction(
    VkDebugReportFlagsEXT       flags,
    VkDebugReportObjectTypeEXT  objectType,
    uint64_t                    object,
    size_t                      location,
    int32_t                     messageCode,
    const char*                 pLayerPrefix,
    const char*                 pMessage,
    void*                       pUserData)
{
    if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        switch (messageCode)
        {
        case 2: // Unused vertex output
        case 3: // Unused vertex attribute.
            return VK_FALSE;
        default:
            // Ignore this case.
            break;
        }

        // Since Vulkan 1.1.82, the message code is not used anymore.
        if(strstr(pMessage, "UNASSIGNED-CoreValidation-Shader-OutputNotConsumed"))
            return VK_FALSE;

        auto device = reinterpret_cast<AVkDevice*> (pUserData);
        if(device->vrSystemWrapper)
        {
            if(strstr(pMessage, "UNASSIGNED-CoreValidation-DrawState-InvalidImageLayout") &&
                strstr(pMessage, "of GENERAL"))
                return VK_FALSE;
        }
    }

    printError("%s: %d: %s\n", pLayerPrefix, messageCode, pMessage);
    return VK_FALSE;
}

AVkDeviceSharedContext::AVkDeviceSharedContext()
    :
	debugReportCallback(VK_NULL_HANDLE)
{
    hasDebugReportExtension = false;
    debugReportCallback = VK_NULL_HANDLE;
    vrSystem = nullptr;
}

AVkDeviceSharedContext::~AVkDeviceSharedContext()
{
    if(vrSystem)
        vr::VR_Shutdown();

    // Destroy the memory allocator.
    if(memoryAllocator)
        vmaDestroyAllocator(memoryAllocator);

    // Destroy the debug report callback.
    if (debugReportCallback)
        fpDestroyDebugReportCallbackEXT(vulkanInstance, debugReportCallback, nullptr);

    // Destroy the vulkan devices
    if(device)
        vkDestroyDevice(device, nullptr);

    if(vulkanInstance)
        vkDestroyInstance(vulkanInstance, nullptr);
}

AVkDevice::AVkDevice()
    :
    implicitResourceSetupCommandList(*this),
    implicitResourceUploadCommandList(*this),
    implicitResourceReadbackCommandList(*this)
{
    vulkanInstance = nullptr;
    physicalDevice = nullptr;
    device = nullptr;

    isVRDisplaySupported = false;
    isVRInputDevicesSupported = false;
}

AVkDevice::~AVkDevice()
{
	// Destroy the implicit command list.
	implicitResourceSetupCommandList.destroy();
	implicitResourceUploadCommandList.destroy();
	implicitResourceReadbackCommandList.destroy();
}

bool AVkDevice::checkVulkanImplementation(VulkanPlatform *platform)
{
    std::vector<std::string> requiredInstanceExtensions(coreRequiredInstanceExtensionNames, coreRequiredInstanceExtensionNames + coreRequiredInstanceExtensionCount);
    std::vector<std::string> requiredDeviceExtensions(coreRequiredDeviceExtensionNames, coreRequiredDeviceExtensionNames + coreRequiredDeviceExtensionCount);

    // Check the required extensions
    uint32_t instanceExtensionCount;
    auto error = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    if (error || instanceExtensionCount == 0)
        return false;

    std::vector<VkExtensionProperties> instanceExtensionProperties(instanceExtensionCount);
    error = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, &instanceExtensionProperties[0]);
    if (error)
        return false;

    if (!hasRequiredExtensions(instanceExtensionProperties, requiredInstanceExtensions))
        return false;

    VkApplicationInfo applicationInfo;
    memset(&applicationInfo, 0, sizeof(applicationInfo));
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    if (!applicationInfo.pApplicationName)
        applicationInfo.pApplicationName = "Generic Abstract GPU Application";
    if (!applicationInfo.pEngineName)
        applicationInfo.pEngineName = "Abstract GPU";
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // Create the instace
    VkInstance vulkanInstance;
    error = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);
    if (error)
        return false;

    // Get the physical devices
    platform->gpuCount = 0;
    error = vkEnumeratePhysicalDevices(vulkanInstance, &platform->gpuCount, nullptr);
    if (error || platform->gpuCount == 0)
    {
        vkDestroyInstance(vulkanInstance, nullptr);
        return false;
    }
    //printf("gpuCount: %d\n", theVulkanPlatform.gpuCount);

    vkDestroyInstance(vulkanInstance, nullptr);

    return true;
}


agpu::device_ref AVkDevice::open(agpu_device_open_info* openInfo)
{
    auto device = agpu::makeObject<AVkDevice> ();
    if (!deviceForVk->initialize(openInfo))
        return agpu::device_ref();

    return device;
}

bool AVkDevice::checkDebugReportExtension()
{
    GET_INSTANCE_PROC_ADDR(CreateDebugReportCallbackEXT);
    GET_INSTANCE_PROC_ADDR(DestroyDebugReportCallbackEXT);

    VkDebugReportCallbackCreateInfoEXT debugInfo;
    memset(&debugInfo, 0, sizeof(debugInfo));
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
    debugInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT |
        VK_DEBUG_REPORT_WARNING_BIT_EXT |
        VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    debugInfo.pfnCallback = debugReportCallbackFunction;
    debugInfo.pUserData = this;

    sharedContext->fpDestroyDebugReportCallbackEXT = fpDestroyDebugReportCallbackEXT;
    auto error = fpCreateDebugReportCallbackEXT(vulkanInstance, &debugInfo, nullptr, &sharedContext->debugReportCallback);
    if (error)
    {
        printError("Failed to register debug report callback.\n");
        return false;
    }

    return true;
}

static void splitSpacesInto(const std::string &string, std::vector<std::string> &dest)
{
    size_t currentPosition = 0;
    for(;;)
    {
        auto endPosition = string.find(' ', currentPosition);
        auto substring = string.substr(currentPosition, endPosition - currentPosition);
        if(!substring.empty())
            dest.push_back(substring);

        if(endPosition >= string.size())
            break;

        currentPosition = endPosition + 1;
    }
}

bool AVkDevice::getInstanceExtensionsRequiredForVR(std::vector<std::string> &requiredInstanceExtensions)
{
    auto compositor = vr::VRCompositor();

    size_t extensionStringBufferSize = compositor->GetVulkanInstanceExtensionsRequired(nullptr, 0);
    std::unique_ptr<char[]> extensionString(new char[extensionStringBufferSize]);

    auto success = compositor->GetVulkanInstanceExtensionsRequired(extensionString.get(), extensionStringBufferSize);
    if(!success)
        return false;

    splitSpacesInto(extensionString.get(), requiredInstanceExtensions);
    return true;
}


bool AVkDevice::getDeviceExtensionsRequiredForVR(VkPhysicalDevice physicalDevice, std::vector<std::string> &requiredDeviceExtensions)
{
    auto compositor = vr::VRCompositor();

    size_t extensionStringBufferSize = compositor->GetVulkanDeviceExtensionsRequired(physicalDevice, nullptr, 0);
    std::unique_ptr<char[]> extensionString(new char[extensionStringBufferSize]);

    auto success = compositor->GetVulkanDeviceExtensionsRequired(physicalDevice, extensionString.get(), extensionStringBufferSize);
    if(!success)
        return false;

    splitSpacesInto(extensionString.get(), requiredDeviceExtensions);
    return true;
}

bool AVkDevice::initialize(agpu_device_open_info* openInfo)
{
    std::vector<std::string> requiredInstanceExtensions(coreRequiredInstanceExtensionNames, coreRequiredInstanceExtensionNames + coreRequiredInstanceExtensionCount);
    std::vector<std::string> requiredDeviceExtensions(coreRequiredDeviceExtensionNames, coreRequiredDeviceExtensionNames + coreRequiredDeviceExtensionCount);

    std::vector<const char *> instanceLayers;
    std::vector<const char *> instanceExtensions;

    std::vector<const char *> deviceLayers;
    std::vector<const char *> deviceExtensions;

    displayHandle = openInfo->display;
    sharedContext = std::make_shared<AVkDeviceSharedContext> ();
    auto &vrSystem = sharedContext->vrSystem;

    // Is VR support requested? if so, we may need to request some extensions
    // that are required by the VR system.
    if((openInfo->open_flags & AGPU_DEVICE_OPEN_FLAG_ALLOW_VR) != 0 && vr::VR_IsHmdPresent())
    {
        vr::HmdError hmdError;
        vrSystem = vr::VR_Init(&hmdError, vr::VRApplication_Scene);
        if(!vrSystem || hmdError != 0)
        {
            printError("VR API initialization error code %d\n", hmdError);
            vrSystem = nullptr;
        }
        else if(!getInstanceExtensionsRequiredForVR(requiredInstanceExtensions))
        {
            vr::VR_Shutdown();
            vrSystem = nullptr;
            printError("Failed to retrieve the required Vulkan extension for VR\n");
        }
    }

    // The application info.
    VkApplicationInfo applicationInfo;
    memset(&applicationInfo, 0, sizeof(applicationInfo));
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = openInfo->application_name;
    applicationInfo.applicationVersion = openInfo->application_version;
    applicationInfo.pEngineName = openInfo->engine_name;
    applicationInfo.engineVersion = openInfo->engine_version;
	applicationInfo.apiVersion = VK_API_VERSION_1_0;
    if (!applicationInfo.pApplicationName)
        applicationInfo.pApplicationName = "Generic Abstract GPU Application";
    if (!applicationInfo.pEngineName)
        applicationInfo.pEngineName = "Abstract GPU";

    VkInstanceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

    // Set the instance layers
    uint32_t instanceLayerCount;
    auto error = vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
    if (error)
        return false;

    if (instanceLayerCount > 0)
    {
        instanceLayerProperties.resize(instanceLayerCount);
        error = vkEnumerateInstanceLayerProperties(&instanceLayerCount, &instanceLayerProperties[0]);
        if (error)
            return false;

        if (openInfo->debug_layer && hasValidationLayers(instanceLayerProperties))
        {
            for (size_t i = 0; i < validationLayerCount; ++i)
                instanceLayers.push_back(validationLayerNames[i]);
        }
    }

    // Get the extensions
    uint32_t instanceExtensionCount;
    error = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
    if (error || instanceExtensionCount == 0)
        return false;

    instanceExtensionProperties.resize(instanceExtensionCount);
    error = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, &instanceExtensionProperties[0]);
    if (error)
        return false;

    if (!hasRequiredExtensions(instanceExtensionProperties, requiredInstanceExtensions))
    {
        printError("Required extensions are missing.\n");
        return false;
    }

    // Enable the required extensions
    for(auto &extension: requiredInstanceExtensions)
        instanceExtensions.push_back(extension.c_str());

    // Enable the debug reporting extension
    if (openInfo->debug_layer && hasExtension("VK_EXT_debug_report", instanceExtensionProperties))
    {
        sharedContext->hasDebugReportExtension = true;
        instanceExtensions.push_back("VK_EXT_debug_report");
    }

    // Set the enabled layers and extensions.
    if (!instanceLayers.empty())
    {
        createInfo.ppEnabledLayerNames = &instanceLayers[0];
        createInfo.enabledLayerCount = (uint32_t)instanceLayers.size();
    }

    createInfo.ppEnabledExtensionNames = &instanceExtensions[0];
    createInfo.enabledExtensionCount = (uint32_t)instanceExtensions.size();

    // Create the instace
    error = vkCreateInstance(&createInfo, nullptr, &vulkanInstance);
    if (error)
        return false;
    sharedContext->vulkanInstance = vulkanInstance;

    // Get the physical devices
    uint32_t gpuCount;
    error = vkEnumeratePhysicalDevices(vulkanInstance, &gpuCount, nullptr);
    if (error || gpuCount == 0)
    {
        printError("Failed to enumerate the gpus.\n");
        return false;
    }

    physicalDevices.resize(gpuCount);
    error = vkEnumeratePhysicalDevices(vulkanInstance, &gpuCount, &physicalDevices[0]);
    if (error)
    {
        printError("Failed to enumerate the gpus.\n");
        return false;
    }

    auto gpuIndex = openInfo->gpu_index;
    if (gpuIndex < 0)
        gpuIndex = 0;
    if (size_t(gpuIndex) >= physicalDevices.size())
    {
        printError("Invalid selected gpu index.\n");
        return false;
    }

    physicalDevice = physicalDevices[gpuIndex];

    // Try to use the physical device required by the VR system.
    if(vrSystem)
    {
        uint64_t vrPhysicalDevice = 0;
        vrSystem->GetOutputDevice(&vrPhysicalDevice, vr::TextureType_Vulkan, vulkanInstance);

        physicalDevice = VK_NULL_HANDLE;
        for(auto device : physicalDevices)
        {
            if(((VkPhysicalDevice)vrPhysicalDevice) == device)
            {
                physicalDevice = device;
                break;
            }
        }

        if(physicalDevice == VK_NULL_HANDLE)
        {
            physicalDevice = physicalDevices[gpuIndex];
            printError("Failed to find the physical device required for VR. Falling back to vulkan physical device number %d.\n", gpuIndex);
        }
    }

    // Get the device features.
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    //printMessage("Vulkan physical device: %s [%08x:%08x %d]\n", deviceProperties.deviceName, deviceProperties.vendorID, deviceProperties.deviceID, deviceProperties.deviceType);

    uint32_t deviceLayerCount;
    error = vkEnumerateDeviceLayerProperties(physicalDevice, &deviceLayerCount, nullptr);
    if (error)
        return false;

    if(deviceLayerCount > 0)
    {
        deviceLayerProperties.resize(deviceLayerCount);
        error = vkEnumerateDeviceLayerProperties(physicalDevice, &deviceLayerCount, &deviceLayerProperties[0]);
        if (error)
            return false;
    }

    uint32_t deviceExtensionCount;
    error = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
    if (error)
        return false;

    if(deviceExtensionCount > 0)
    {
        deviceExtensionProperties.resize(deviceExtensionCount);
        error = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, &deviceExtensionProperties[0]);
        if (error)
            return false;
    }

    if(vrSystem)
    {
        if(!getDeviceExtensionsRequiredForVR(physicalDevice, requiredDeviceExtensions))
        {
            vr::VR_Shutdown();
            vrSystem = nullptr;
        }
    }

    if (!hasRequiredExtensions(deviceExtensionProperties, requiredDeviceExtensions))
    {
        printError("The device is missing some required extensions.");
        return false;
    }

    // Enable the required device extensions.
    for(auto &extension: requiredDeviceExtensions)
        deviceExtensions.push_back(extension.c_str());

    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    if (queueFamilyCount == 0)
        return false;

    queueProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, &queueProperties[0]);

    GET_INSTANCE_PROC_ADDR(GetDeviceProcAddr);

    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfaceSupportKHR);
    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfaceCapabilitiesKHR);
    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfaceFormatsKHR);
    GET_INSTANCE_PROC_ADDR(GetPhysicalDeviceSurfacePresentModesKHR);
    GET_INSTANCE_PROC_ADDR(GetSwapchainImagesKHR);

    if (sharedContext->hasDebugReportExtension)
        sharedContext->hasDebugReportExtension = checkDebugReportExtension();

    // Open all the availables queues.
    std::vector<VkDeviceQueueCreateInfo> createQueueInfos;

    uint32_t maxQueueCount = 0;
    for (size_t i = 0; i < queueFamilyCount; ++i)
        maxQueueCount = std::max(maxQueueCount, queueProperties[i].queueCount);

    std::vector<float> queuePriorities(maxQueueCount);
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        auto &queueProps = queueProperties[i];
        VkDeviceQueueCreateInfo createQueueInfo;
        memset(&createQueueInfo, 0, sizeof(createQueueInfo));

        createQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createQueueInfo.queueFamilyIndex = i;
        createQueueInfo.queueCount = queueProps.queueCount;
        createQueueInfo.pQueuePriorities = &queuePriorities[0];
        createQueueInfos.push_back(createQueueInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo;
    memset(&deviceCreateInfo, 0, sizeof(deviceCreateInfo));
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = (uint32_t)createQueueInfos.size();
    deviceCreateInfo.pQueueCreateInfos = &createQueueInfos[0];
    deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

    if (openInfo->debug_layer && hasValidationLayers(deviceLayerProperties))
    {
        for (size_t i = 0; i < validationLayerCount; ++i)
            deviceLayers.push_back(validationLayerNames[i]);
    }

    // Set the device layers and extensions
    deviceCreateInfo.ppEnabledExtensionNames = &deviceExtensions[0];
    deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();

    if (!deviceLayers.empty())
    {
        deviceCreateInfo.ppEnabledLayerNames = &deviceLayers[0];
        deviceCreateInfo.enabledLayerCount = (uint32_t)deviceLayers.size();
    }

    // Create the device.
    error = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    if (error)
        return false;
    sharedContext->device = device;

    GET_DEVICE_PROC_ADDR(CreateSwapchainKHR);
    GET_DEVICE_PROC_ADDR(DestroySwapchainKHR);
    GET_DEVICE_PROC_ADDR(GetSwapchainImagesKHR);
    GET_DEVICE_PROC_ADDR(AcquireNextImageKHR);
    GET_DEVICE_PROC_ADDR(QueuePresentKHR);

    // Get the queues.
    for (uint32_t i = 0; i < queueFamilyCount; ++i)
    {
        auto &familyProperties = queueProperties[i];
        auto isGraphics = (familyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
        auto isCompute = (familyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;
        auto isTransfer = (familyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) != 0;
        if (!isGraphics && !isCompute && !isTransfer)
            continue;

        agpu_command_queue_type queueType;
        if (isGraphics)
            queueType = AGPU_COMMAND_QUEUE_TYPE_GRAPHICS;
        else if(isCompute)
            queueType = AGPU_COMMAND_QUEUE_TYPE_COMPUTE;
        else //if(isTransfer)
            queueType = AGPU_COMMAND_QUEUE_TYPE_TRANSFER;

        for (uint32_t j = 0; j < familyProperties.queueCount; ++j)
        {
            VkQueue queue;
            vkGetDeviceQueue(device, i, j, &queue);

            auto commandQueue = AVkCommandQueue::create(refFromThis<agpu::device> (), i, j, queue, queueType);
            if (!commandQueue)
                continue;

            switch (queueType)
            {
            case AGPU_COMMAND_QUEUE_TYPE_GRAPHICS:
                graphicsCommandQueues.push_back(commandQueue);
                break;
            case AGPU_COMMAND_QUEUE_TYPE_COMPUTE:
                computeCommandQueues.push_back(commandQueue);
                break;
            case AGPU_COMMAND_QUEUE_TYPE_TRANSFER:
                transferCommandQueues.push_back(commandQueue);
                break;
            default:
                break;
            }
        }
    }

    // Initialize the memory allocator.
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.instance = vulkanInstance;
    allocatorInfo.physicalDevice = physicalDevice;
    allocatorInfo.device = device;
    vmaCreateAllocator(&allocatorInfo, &sharedContext->memoryAllocator);

    // Store a copy to in the implicit resource command lists.
    implicitResourceSetupCommandList.commandQueue = graphicsCommandQueues[0];
    implicitResourceUploadCommandList.commandQueue = graphicsCommandQueues[0];
    implicitResourceReadbackCommandList.commandQueue = graphicsCommandQueues[0];

    // Create the VR system.
    if(vrSystem)
    {
        vrSystemWrapper = agpu::makeObject<AVkVrSystem> (refFromThis<agpu::device> ());
        if(!vrSystemWrapper.as<AVkVrSystem> ()->initialize())
            vrSystemWrapper.reset();

        isVRDisplaySupported = (bool)vrSystemWrapper;
    }

    return true;
}

agpu_bool AVkDevice::isFeatureSupported(agpu_feature feature)
{
	switch (feature)
	{
	case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COMMAND_LIST_REUSE: return true;
	case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE: return false;
    case AGPU_FEATURE_VRDISPLAY: return isVRDisplaySupported;
    case AGPU_FEATURE_VRINPUT_DEVICES: return isVRInputDevicesSupported;
	default: return false;
	}
}


agpu_int AVkDevice::getLimitValue(agpu_limit limit)
{
    switch(limit)
    {
    case AGPU_LIMIT_NON_COHERENT_ATOM_SIZE: return deviceProperties.limits.nonCoherentAtomSize;
    case AGPU_LIMIT_MIN_MEMORY_MAP_ALIGNMENT: return deviceProperties.limits.minMemoryMapAlignment;
    case AGPU_LIMIT_MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT: return deviceProperties.limits.minTexelBufferOffsetAlignment;
    case AGPU_LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT: return deviceProperties.limits.minUniformBufferOffsetAlignment;
    case AGPU_LIMIT_MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT: return deviceProperties.limits.minStorageBufferOffsetAlignment;
    default: return 0;
    }
}

agpu_int AVkDevice::getMultiSampleQualityLevels(agpu_texture_format format, agpu_uint sample_count)
{
    return 1;
}

agpu::command_queue_ptr AVkDevice::getDefaultCommandQueue()
{
    return getGraphicsCommandQueue(0);
}

agpu::command_queue_ptr AVkDevice::getGraphicsCommandQueue(agpu_uint index)
{
    if (index >= graphicsCommandQueues.size())
        return nullptr;

    return graphicsCommandQueues[index].disownedNewRef();
}

agpu::command_queue_ptr AVkDevice::getComputeCommandQueue(agpu_uint index)
{
    if (index >= computeCommandQueues.size())
        return nullptr;

    return computeCommandQueues[index].disownedNewRef();
}

agpu::command_queue_ptr AVkDevice::getTransferCommandQueue(agpu_uint index)
{
    if (index >= transferCommandQueues.size())
        return nullptr;

    return transferCommandQueues[index].disownedNewRef();
}

agpu::swap_chain_ptr AVkDevice::createSwapChain(const agpu::command_queue_ref &commandQueue, agpu_swap_chain_create_info* swapChainInfo)
{
    return AVkSwapChain::create(refFromThis<agpu::device> (), commandQueue, swapChainInfo).disown();
}

agpu::buffer_ptr AVkDevice::createBuffer(agpu_buffer_description* description, agpu_pointer initial_data)
{
    return AVkBuffer::create(refFromThis<agpu::device> (), description, initial_data).disown();
}

agpu::vertex_layout_ptr AVkDevice::createVertexLayout()
{
    return AVkVertexLayout::create(refFromThis<agpu::device> ()).disown();
}

agpu::vertex_binding_ptr AVkDevice::createVertexBinding(const agpu::vertex_layout_ref & layout)
{
    return AVkVertexBinding::create(refFromThis<agpu::device> (), layout).disown();
}

agpu::shader_ptr AVkDevice::createShader(agpu_shader_type type)
{
    return AVkShader::create(refFromThis<agpu::device> (), type).disown();
}

agpu::shader_signature_builder_ptr AVkDevice::createShaderSignatureBuilder()
{
    return AVkShaderSignatureBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::pipeline_builder_ptr AVkDevice::createPipelineBuilder()
{
    return AVkGraphicsPipelineBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::compute_pipeline_builder_ptr AVkDevice::createComputePipelineBuilder()
{
    return AVkComputePipelineBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::command_allocator_ptr AVkDevice::createCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & queue)
{
    return AVkCommandAllocator::create(refFromThis<agpu::device> (), type, queue).disown();
}

agpu::command_list_ptr AVkDevice::createCommandList(agpu_command_list_type type, const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state)
{
    return AVkCommandList::create(refFromThis<agpu::device> (), type, allocator, initial_pipeline_state).disown();
}

agpu_shader_language AVkDevice::getPreferredShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_SPIR_V;
}

agpu_shader_language AVkDevice::getPreferredIntermediateShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_SPIR_V;
}

agpu_shader_language AVkDevice::getPreferredHighLevelShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_NONE;
}

agpu::framebuffer_ptr AVkDevice::createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref & depthStencilView)
{
    return AVkFramebuffer::create(refFromThis<agpu::device> (), width, height, colorCount, colorViews, depthStencilView).disown();
}

agpu::renderpass_ptr AVkDevice::createRenderPass(agpu_renderpass_description* description)
{
    return AVkRenderPass::create(refFromThis<agpu::device> (), description).disown();
}

agpu::texture_ptr AVkDevice::createTexture(agpu_texture_description* description)
{
    return AVkTexture::create(refFromThis<agpu::device> (), description).disown();
}

agpu::sampler_ptr AVkDevice::createSampler(agpu_sampler_description* description)
{
    return AVkSampler::create(refFromThis<agpu::device> (), description).disown();
}

agpu::fence_ptr AVkDevice::createFence()
{
    return AVkFence::create(refFromThis<agpu::device> ()).disown();
}

AGPU_EXPORT agpu_bool agpuHasBottomLeftTextureCoordinates(agpu_device *device)
{
    return false;
}

agpu_bool AVkDevice::hasTopLeftNdcOrigin()
{
    return true;
}

agpu_bool AVkDevice::hasBottomLeftTextureCoordinates()
{
    return false;
}

agpu::vr_system_ptr AVkDevice::getVRSystem()
{
    return vrSystemWrapper.disownedNewRef();
}

agpu::offline_shader_compiler_ptr AVkDevice::createOfflineShaderCompiler()
{
	return AgpuCommon::GLSLangOfflineShaderCompiler::createForDevice(refFromThis<agpu::device> ()).disown();
}

agpu::state_tracker_cache_ptr AVkDevice::createStateTrackerCache(const agpu::command_queue_ref & command_queue_family)
{
	return AgpuCommon::StateTrackerCache::create(refFromThis<agpu::device> (), 0).disown();
}

agpu_error AVkDevice::finishExecution()
{
    vkDeviceWaitIdle(device);
    return AGPU_OK;
}
} // End of namespace AgpuVulkan
