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
    if(!GetConsoleCP())
        OutputDebugStringA(buffer);
    else
        fputs(buffer, stderr);
#else
    fputs(buffer, stderr);
#endif
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

AVkDevice::AVkDevice()
{
    vulkanInstance = nullptr;
    physicalDevice = nullptr;
    device = nullptr;
    setupCommandPool = VK_NULL_HANDLE;
    setupCommandBuffer = nullptr;
    hasDebugReportExtension = false;
    debugReportCallback = VK_NULL_HANDLE;

    isVRDisplaySupported = false;
    isVRInputDevicesSupported = false;
    vrSystem = nullptr;
}

AVkDevice::~AVkDevice()
{
    // Shutdown the VR system, when I die.
    if(vrSystem)
        vr::VR_Shutdown();
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

    auto error = fpCreateDebugReportCallbackEXT(vulkanInstance, &debugInfo, nullptr, &debugReportCallback);
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
        hasDebugReportExtension = true;
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

    deviceLayerProperties.resize(deviceLayerCount);
    error = vkEnumerateDeviceLayerProperties(physicalDevice, &deviceLayerCount, &deviceLayerProperties[0]);
    if (error)
        return false;

    uint32_t deviceExtensionCount;
    error = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
    if (error)
        return false;

    deviceExtensionProperties.resize(deviceExtensionCount);
    error = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, &deviceExtensionProperties[0]);
    if (error)
        return false;

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

    if (hasDebugReportExtension)
        hasDebugReportExtension = checkDebugReportExtension();

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

    setupQueue = graphicsCommandQueues[0];

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

agpu_int AVkDevice::getMultiSampleQualityLevels(agpu_uint sample_count)
{
    return 0;
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

agpu::framebuffer_ptr AVkDevice::createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
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

bool AVkDevice::createSetupCommandBuffer()
{
    VkCommandPoolCreateInfo poolCreate;
    memset(&poolCreate, 0, sizeof(poolCreate));
    poolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreate.queueFamilyIndex = setupQueue.as<AVkCommandQueue> ()->queueFamilyIndex;
    poolCreate.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    auto error = vkCreateCommandPool(device, &poolCreate, nullptr, &setupCommandPool);
    if (error)
        return false;

    VkCommandBufferAllocateInfo commandInfo;
    memset(&commandInfo, 0, sizeof(commandInfo));
    commandInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandInfo.commandPool = setupCommandPool;
    commandInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandInfo.commandBufferCount = 1;

    error = vkAllocateCommandBuffers(device, &commandInfo, &setupCommandBuffer);
    if (error)
    {
        vkDestroyCommandPool(device, setupCommandPool, nullptr);
        setupCommandPool = VK_NULL_HANDLE;
        return false;
    }

    VkCommandBufferInheritanceInfo inheritance;
    memset(&inheritance, 0, sizeof(inheritance));
    inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo beginInfo;
    memset(&beginInfo, 0, sizeof(beginInfo));
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = &inheritance;

    error = vkBeginCommandBuffer(setupCommandBuffer, &beginInfo);
    if (error)
    {
        vkFreeCommandBuffers(device, setupCommandPool, 1, &setupCommandBuffer);
        vkDestroyCommandPool(device, setupCommandPool, nullptr);
        setupCommandPool = VK_NULL_HANDLE;
        setupCommandBuffer = VK_NULL_HANDLE;
        return false;
    }

    return true;
}

bool AVkDevice::setImageLayout(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask)
{
    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    auto barrier = barrierForImageLayoutTransition(image, range, aspect, sourceLayout, destLayout, srcAccessMask, srcStages, destStages);

    vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    return submitSetupCommandBuffer();
}

bool AVkDevice::clearImageWithColor(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask, VkClearColorValue *clearValue)
{
    range.aspectMask = aspect;

    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    // Transition to dst optimal
    VkImageLayout transferLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    if (sourceLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, sourceLayout, transferLayout, srcAccessMask, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    // Clear the image
    vkCmdClearColorImage(setupCommandBuffer, image, transferLayout, clearValue, 1, &range);

    // Transition to target layout
    if (destLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, transferLayout, destLayout, VK_ACCESS_TRANSFER_WRITE_BIT, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    return submitSetupCommandBuffer();
}

bool AVkDevice::clearImageWithDepthStencil(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask, VkClearDepthStencilValue *clearValue)
{
    range.aspectMask = aspect;

    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    // Transition to dst optimal
    VkImageLayout transferLayout = destLayout == VK_IMAGE_LAYOUT_GENERAL ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    if (destLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, sourceLayout, transferLayout, srcAccessMask, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    // Clear the image
    vkCmdClearDepthStencilImage(setupCommandBuffer, image, transferLayout, clearValue, 1, &range);

    // Transition to target layout
    if (destLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, transferLayout, destLayout, VK_ACCESS_TRANSFER_WRITE_BIT, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    return submitSetupCommandBuffer();
}

bool AVkDevice::copyBuffer(VkBuffer sourceBuffer, VkBuffer destBuffer, uint32_t regionCount, const VkBufferCopy *regions)
{
    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    vkCmdCopyBuffer(setupCommandBuffer, sourceBuffer, destBuffer, regionCount, regions);
    return submitSetupCommandBuffer();
}

bool AVkDevice::copyBufferToImage(VkBuffer buffer, VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout destLayout, VkAccessFlags destAccessMask, uint32_t regionCount, const VkBufferImageCopy *regions)
{
    range.aspectMask = aspect;

    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    VkImageLayout transferLayout = destLayout == VK_IMAGE_LAYOUT_GENERAL ? VK_IMAGE_LAYOUT_GENERAL : VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    if (destLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, destLayout, transferLayout, destAccessMask, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    vkCmdCopyBufferToImage(setupCommandBuffer, buffer, image, transferLayout, regionCount, regions);

    if (destLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, transferLayout, destLayout, VK_ACCESS_TRANSFER_WRITE_BIT, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    return submitSetupCommandBuffer();
}

bool AVkDevice::copyImageToBuffer(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout destLayout, VkAccessFlags destAccessMask, VkBuffer buffer, uint32_t regionCount, const VkBufferImageCopy *regions)
{
    range.aspectMask = aspect;

    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    VkImageLayout transferLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    if (destLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, destLayout, transferLayout, destAccessMask, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    vkCmdCopyImageToBuffer(setupCommandBuffer, image, transferLayout, buffer, regionCount, regions);

    if (destLayout != transferLayout)
    {
        VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        auto barrier = barrierForImageLayoutTransition(image, range, aspect, transferLayout, destLayout, VK_ACCESS_TRANSFER_READ_BIT, srcStages, destStages);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    return submitSetupCommandBuffer();
}

bool AVkDevice::submitSetupCommandBuffer()
{
    auto error = vkEndCommandBuffer(setupCommandBuffer);
    if (error)
        abort();

    VkSubmitInfo submitInfo;
    memset(&submitInfo, 0, sizeof(submitInfo));
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &setupCommandBuffer;

    error = vkQueueSubmit(setupQueue.as<AVkCommandQueue> ()->queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (error)
        abort();

    error = vkQueueWaitIdle(setupQueue.as<AVkCommandQueue> ()->queue);
    if (error)
        abort();

    error = vkResetCommandPool(device, setupCommandPool, 0);
    if (error)
        abort();

    VkCommandBufferInheritanceInfo inheritance;
    memset(&inheritance, 0, sizeof(inheritance));
    inheritance.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

    VkCommandBufferBeginInfo beginInfo;
    memset(&beginInfo, 0, sizeof(beginInfo));
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = &inheritance;

    error = vkBeginCommandBuffer(setupCommandBuffer, &beginInfo);
    if (error)
    {
        vkFreeCommandBuffers(device, setupCommandPool, 1, &setupCommandBuffer);
        vkDestroyCommandPool(device, setupCommandPool, nullptr);
        setupCommandPool = VK_NULL_HANDLE;
        setupCommandBuffer = VK_NULL_HANDLE;
        return false;
    }

    return true;
}

inline void addImageLayoutBarrierMasks(VkImageLayout layout, VkAccessFlags &accessMask, VkPipelineStageFlags &stages)
{
    switch(layout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        accessMask = VK_ACCESS_TRANSFER_READ_BIT;
        stages |= VK_PIPELINE_STAGE_TRANSFER_BIT;
        break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        accessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        stages |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
        accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
        stages |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;
    case VK_IMAGE_LAYOUT_GENERAL:
        accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
        stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
        break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
        stages |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        break;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        accessMask = VK_ACCESS_MEMORY_READ_BIT;
        break;
    default:
        break;
    }
}

VkImageMemoryBarrier AVkDevice::barrierForImageLayoutTransition(VkImage image, VkImageSubresourceRange range, VkImageAspectFlags aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlags srcAccessMask, VkPipelineStageFlags &srcStages, VkPipelineStageFlags &dstStages)
{
    VkImageMemoryBarrier barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcAccessMask = srcAccessMask;
    barrier.dstAccessMask = 0;
    barrier.oldLayout = sourceLayout;
    barrier.newLayout = destLayout;
    barrier.subresourceRange = range;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    addImageLayoutBarrierMasks(barrier.oldLayout, barrier.srcAccessMask, srcStages);
    addImageLayoutBarrierMasks(barrier.newLayout, barrier.dstAccessMask, dstStages);
    return barrier;
}

} // End of namespace AgpuVulkan
