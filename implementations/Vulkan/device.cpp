#include "device.hpp"
#include "command_queue.hpp"
#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"
#include "buffer.hpp"

#define GET_INSTANCE_PROC_ADDR(procName) \
    {                                                                          \
        fp##procName = (PFN_vk##procName)vkGetInstanceProcAddr(vulkanInstance, "vk" #procName); \
        if (fp##procName == NULL) {                                    \
            printError("vkGetInstanceProcAddr failed to find vk" #procName, "vkGetInstanceProcAddr Failure");                         \
            return false; \
        }                                                                      \
    }

#define GET_DEVICE_PROC_ADDR(procName) \
    {                                                                          \
        fp##procName = (PFN_vk##procName)fpGetDeviceProcAddr(device, "vk" #procName); \
        if (fp##procName == NULL) {                                    \
            printError("vkGetDeviceProcAddr failed to find vk" #procName, "vkGetInstanceProcAddr Failure");                         \
            return false; \
        }                                                                      \
    }

void printError(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
#ifdef _WIN32
    OutputDebugStringA(buffer);
#else
    fputs(buffer, stderr);
#endif
    va_end(args);
}

const char *validationLayerNames[] = {
    "VK_LAYER_LUNARG_draw_state",
    "VK_LAYER_LUNARG_param_checker",
    "VK_LAYER_LUNARG_image",
    "VK_LAYER_LUNARG_mem_tracker",
    "VK_LAYER_LUNARG_object_tracker",
    "VK_LAYER_LUNARG_threading",
    "VK_LAYER_LUNARG_device_limits",
    "VK_LAYER_LUNARG_swapchain",
};

constexpr size_t validationLayerCount = sizeof(validationLayerNames) / sizeof(validationLayerNames[0]);

const char *requiredExtensionNames[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
};

constexpr size_t requiredExtensionCount = sizeof(requiredExtensionNames) / sizeof(requiredExtensionNames[0]);

const char *requiredDeviceExtensionNames[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

constexpr size_t requiredDeviceExtensionCount = sizeof(requiredDeviceExtensionNames) / sizeof(requiredDeviceExtensionNames[0]);


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

static bool hasRequiredExtensions(const std::vector<VkExtensionProperties> &extensions)
{
    for (size_t i = 0; i < requiredExtensionCount; ++i)
    {
        if (!hasExtension(requiredExtensionNames[i], extensions))
            return false;
    }

    return true;
}

static bool hasRequiredDeviceExtensions(const std::vector<VkExtensionProperties> &extensions)
{
    for (size_t i = 0; i < requiredDeviceExtensionCount; ++i)
    {
        if (!hasExtension(requiredDeviceExtensionNames[i], extensions))
            return false;
    }

    return true;
}

_agpu_device::_agpu_device()
{
    vulkanInstance = nullptr;
    physicalDevice = nullptr;
    device = nullptr;
}

void _agpu_device::lostReferences()
{
}

agpu_device *_agpu_device::open(agpu_device_open_info* openInfo)
{
    std::unique_ptr<agpu_device> device(new agpu_device());
    if (!device->initialize(openInfo))
        return nullptr;

    return device.release();
}

bool _agpu_device::initialize(agpu_device_open_info* openInfo)
{
    VkApplicationInfo applicationInfo;
    memset(&applicationInfo, 0, sizeof(applicationInfo));
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = openInfo->application_name;
    applicationInfo.applicationVersion = openInfo->application_version;
    applicationInfo.pEngineName = openInfo->engine_name;
    applicationInfo.engineVersion = openInfo->engine_version;
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
            createInfo.enabledLayerCount = validationLayerCount;
            createInfo.ppEnabledLayerNames = validationLayerNames;
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

    if (!hasRequiredExtensions(instanceExtensionProperties))
    {
        printError("Required extensions are missing.\n");
        return false;
    }

    // Enable the required extensions
    createInfo.ppEnabledExtensionNames = requiredExtensionNames;
    createInfo.enabledExtensionCount = requiredExtensionCount;
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
    if (gpuIndex >= physicalDevices.size())
    {
        printError("Invalid selected gpu index.\n");
        return false;
    }

    physicalDevice = physicalDevices[gpuIndex];
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
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

    if (!hasRequiredDeviceExtensions(deviceExtensionProperties))
    {
        printError("The device is missing some required extensions.");
        return false;
    }

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

    // Open all the availables queues.
    std::vector<VkDeviceQueueCreateInfo> createQueueInfos;
    float queuePriorities[] = { 0.0f };
    for (size_t i = 0; i < queueFamilyCount; ++i)
    {
        auto &queueProps = queueProperties[i];
        VkDeviceQueueCreateInfo createQueueInfo;
        memset(&createQueueInfo, 0, sizeof(createQueueInfo));
        
        createQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        createQueueInfo.queueFamilyIndex = i;
        createQueueInfo.queueCount = queueProps.queueCount;
        createQueueInfo.pQueuePriorities = queuePriorities;
        createQueueInfos.push_back(createQueueInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo;
    memset(&deviceCreateInfo, 0, sizeof(deviceCreateInfo));
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = createQueueInfos.size();
    deviceCreateInfo.pQueueCreateInfos = &createQueueInfos[0];
    deviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensionNames;
    deviceCreateInfo.enabledExtensionCount = requiredDeviceExtensionCount;

    if (openInfo->debug_layer && hasValidationLayers(deviceLayerProperties))
    {
        deviceCreateInfo.ppEnabledLayerNames = validationLayerNames;
        deviceCreateInfo.enabledLayerCount = validationLayerCount;
    }

    error = vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device);
    if (error)
        return false;

    GET_DEVICE_PROC_ADDR(CreateSwapchainKHR);
    GET_DEVICE_PROC_ADDR(DestroySwapchainKHR);
    GET_DEVICE_PROC_ADDR(GetSwapchainImagesKHR);
    GET_DEVICE_PROC_ADDR(AcquireNextImageKHR);
    GET_DEVICE_PROC_ADDR(QueuePresentKHR);

    // Get the queues.
    for (size_t i = 0; i < queueFamilyCount; ++i)
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

        for (size_t j = 0; j < familyProperties.queueCount; ++j)
        {
            VkQueue queue;
            vkGetDeviceQueue(device, i, j, &queue);

            auto commandQueue = agpu_command_queue::create(this, i, j, queue, queueType);
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


    return true;
}

agpu_int _agpu_device::getMultiSampleQualityLevels(agpu_uint sample_count)
{
    return 0;
}

agpu_command_queue* _agpu_device::getDefaultCommandQueue()
{
    return getGraphicsCommandQueue(0);
}

agpu_command_queue* _agpu_device::getGraphicsCommandQueue(agpu_uint index)
{
    if (index >= graphicsCommandQueues.size())
        return nullptr;

    auto result = graphicsCommandQueues[index];
    result->retain();
    return result;
}
agpu_command_queue* _agpu_device::getComputeCommandQueue(agpu_uint index)
{
    if (index >= computeCommandQueues.size())
        return nullptr;

    auto result = computeCommandQueues[index];
    result->retain();
    return result;
}

agpu_command_queue* _agpu_device::getTransferCommandQueue(agpu_uint index)
{
    if (index >= transferCommandQueues.size())
        return nullptr;

    auto result = transferCommandQueues[index];
    result->retain();
    return result;
}

// Exported C API
AGPU_EXPORT agpu_error agpuAddDeviceReference(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->release();
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue(agpu_device* device)
{
    if (!device)
        return nullptr;
    return device->getDefaultCommandQueue();
}

AGPU_EXPORT agpu_command_queue* agpuGetGraphicsCommandQueue(agpu_device* device, agpu_uint index)
{
    if (!device)
        return nullptr;
    return device->getGraphicsCommandQueue(index);
}

AGPU_EXPORT agpu_command_queue* agpuGetComputeCommandQueue(agpu_device* device, agpu_uint index)
{
    if (!device)
        return nullptr;
    return device->getComputeCommandQueue(index);
}

AGPU_EXPORT agpu_command_queue* agpuGetTransferCommandQueue(agpu_device* device, agpu_uint index)
{
    if (!device)
        return nullptr;
    return device->getTransferCommandQueue(index);
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain(agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo)
{
    if (!device)
        return nullptr;

    return agpu_swap_chain::create(device, commandQueue, swapChainInfo);
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    return nullptr;
}

AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout(agpu_device* device)
{
    return nullptr;
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* device, agpu_vertex_layout* layout)
{
    return nullptr;
}

AGPU_EXPORT agpu_shader* agpuCreateShader(agpu_device* device, agpu_shader_type type)
{
    return nullptr;
}

AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder(agpu_device* device)
{
    return nullptr;
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder(agpu_device* device)
{
    return nullptr;
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator(agpu_device* device, agpu_command_list_type type)
{
    return nullptr;
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList(agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    return nullptr;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_SPIR_V;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_NONE;
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
    if (!device)
        return nullptr;

    return agpu_framebuffer::create(device, width, height, colorCount, colorViews, depthStencilView);
}

AGPU_EXPORT agpu_texture* agpuCreateTexture(agpu_device* device, agpu_texture_description* description)
{
    if (!device)
        return nullptr;

    return agpu_texture::create(device, description);
}

AGPU_EXPORT agpu_fence* agpuCreateFence(agpu_device* device)
{
    return nullptr;
}

AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels(agpu_device* device, agpu_uint sample_count)
{
    return AGPU_UNIMPLEMENTED;
}
