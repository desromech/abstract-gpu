#include <algorithm>
#include "device.hpp"
#include "command_queue.hpp"
#include "swap_chain.hpp"
#include "framebuffer.hpp"
#include "texture.hpp"
#include "buffer.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "pipeline_builder.hpp"
#include "shader_signature_builder.hpp"
#include "shader.hpp"
#include "vertex_layout.hpp"
#include "vertex_binding.hpp"
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
    "VK_LAYER_LUNARG_draw_state",
    "VK_LAYER_LUNARG_param_checker",
    "VK_LAYER_LUNARG_image",
    "VK_LAYER_LUNARG_mem_tracker",
    "VK_LAYER_LUNARG_object_tracker",
    //"VK_LAYER_LUNARG_threading",
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
        case 3: // Unused vertex attribute.
        case 50: // Use render pass clear op.
            return VK_FALSE;
        }

    }

    printError("%s\n", pMessage);
    return VK_FALSE;
}

_agpu_device::_agpu_device()
{
    vulkanInstance = nullptr;
    physicalDevice = nullptr;
    device = nullptr;
    setupCommandPool = nullptr;
    setupCommandBuffer = nullptr;
    hasDebugReportExtension = false;
    debugReportCallback = nullptr;
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
    std::vector<const char *> instanceLayers;
    std::vector<const char *> instanceExtensions;
    std::vector<const char *> deviceLayers;
    std::vector<const char *> deviceExtensions;

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

    if (!hasRequiredExtensions(instanceExtensionProperties))
    {
        printError("Required extensions are missing.\n");
        return false;
    }

    // Enable the required extensions
    for (size_t i = 0; i < requiredExtensionCount; ++i)
        instanceExtensions.push_back(requiredExtensionNames[i]);

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
        createInfo.enabledLayerCount = instanceLayers.size();
    }

    createInfo.ppEnabledExtensionNames = &instanceExtensions[0];
    createInfo.enabledExtensionCount = instanceExtensions.size();

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
    if (gpuIndex >= physicalDevices.size())
    {
        printError("Invalid selected gpu index.\n");
        return false;
    }

    physicalDevice = physicalDevices[gpuIndex];
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

    if (!hasRequiredDeviceExtensions(deviceExtensionProperties))
    {
        printError("The device is missing some required extensions.");
        return false;
    }

    // Enable the required device extensions.
    for (size_t i = 0; i < requiredDeviceExtensionCount; ++i)
        deviceExtensions.push_back(requiredDeviceExtensionNames[i]);

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
            printError("Failed to register debug report callback.\n");
    }

    // Open all the availables queues.
    std::vector<VkDeviceQueueCreateInfo> createQueueInfos;
    
    uint32_t maxQueueCount = 0;
    for (size_t i = 0; i < queueFamilyCount; ++i)
        maxQueueCount = std::max(maxQueueCount, queueProperties[i].queueCount);

    std::vector<float> queuePriorities(maxQueueCount);
    for (size_t i = 0; i < queueFamilyCount; ++i)
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
    deviceCreateInfo.queueCreateInfoCount = createQueueInfos.size();
    deviceCreateInfo.pQueueCreateInfos = &createQueueInfos[0];
    if (openInfo->debug_layer && hasValidationLayers(deviceLayerProperties))
    {
        for (size_t i = 0; i < validationLayerCount; ++i)
            deviceLayers.push_back(validationLayerNames[i]);
    }

    // Set the device layers and extensions
    deviceCreateInfo.ppEnabledExtensionNames = &deviceExtensions[0];
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();

    if (!deviceLayers.empty())
    {
        deviceCreateInfo.ppEnabledLayerNames = &deviceLayers[0];
        deviceCreateInfo.enabledLayerCount = deviceLayers.size();
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

    setupQueue = graphicsCommandQueues[0];

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

bool _agpu_device::createSetupCommandBuffer()
{
    VkCommandPoolCreateInfo poolCreate;
    memset(&poolCreate, 0, sizeof(poolCreate));
    poolCreate.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolCreate.queueFamilyIndex = setupQueue->queueFamilyIndex;

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

bool _agpu_device::setImageLayout(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask)
{
    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    auto barrier = barrierForImageLayoutTransition(image, aspect, sourceLayout, destLayout, srcAccessMask);
    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    return submitSetupCommandBuffer();
}

bool _agpu_device::clearImageWithColor(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask, VkClearColorValue *clearValue)
{
    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    // Transition to dst optimal
    {
        auto barrier = barrierForImageLayoutTransition(image, aspect, sourceLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, srcAccessMask);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    // Clear the image
    {
        VkImageSubresourceRange range;
        memset(&range, 0, sizeof(range));
        range.aspectMask = aspect;
        range.layerCount = 1;
        range.levelCount = 1;
        vkCmdClearColorImage(setupCommandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clearValue, 1, &range);
    }

    // Transition to target layout
    {
        auto barrier = barrierForImageLayoutTransition(image, aspect, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, destLayout, VK_ACCESS_TRANSFER_WRITE_BIT);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    return submitSetupCommandBuffer();
}

bool _agpu_device::clearImageWithDepthStencil(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask, VkClearDepthStencilValue *clearValue)
{
    std::unique_lock<std::mutex> l(setupMutex);
    if (!setupCommandBuffer)
    {
        if (!createSetupCommandBuffer())
            return false;
    }

    VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

    // Transition to dst optimal
    {
        auto barrier = barrierForImageLayoutTransition(image, aspect, sourceLayout, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, srcAccessMask);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    // Clear the image
    {
        VkImageSubresourceRange range;
        memset(&range, 0, sizeof(range));
        range.aspectMask = aspect;
        range.layerCount = 1;
        range.levelCount = 1;
        vkCmdClearDepthStencilImage(setupCommandBuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, clearValue, 1, &range);
    }

    // Transition to target layout
    {
        auto barrier = barrierForImageLayoutTransition(image, aspect, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, destLayout, VK_ACCESS_TRANSFER_WRITE_BIT);
        vkCmdPipelineBarrier(setupCommandBuffer, srcStages, destStages, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    }

    return submitSetupCommandBuffer();
}

bool _agpu_device::copyBuffer(VkBuffer sourceBuffer, VkBuffer destBuffer, uint32_t regionCount, const VkBufferCopy *regions)
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

bool _agpu_device::submitSetupCommandBuffer()
{
    auto error = vkEndCommandBuffer(setupCommandBuffer);
    if (error)
        abort();

    VkSubmitInfo submitInfo;
    memset(&submitInfo, 0, sizeof(submitInfo));
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &setupCommandBuffer;

    error = vkQueueSubmit(setupQueue->queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (error)
        abort();

    error = vkQueueWaitIdle(setupQueue->queue);
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

VkImageMemoryBarrier _agpu_device::barrierForImageLayoutTransition(VkImage image, VkImageAspectFlagBits aspect, VkImageLayout sourceLayout, VkImageLayout destLayout, VkAccessFlagBits srcAccessMask)
{
    VkImageMemoryBarrier barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcAccessMask = srcAccessMask;
    barrier.oldLayout = sourceLayout;
    barrier.newLayout = destLayout;
    barrier.subresourceRange.aspectMask = aspect;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    if (destLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    }

    if (destLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    }

    if (destLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    }

    if (destLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
    }

    return barrier;
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
    if (!device)
        return nullptr;

    return agpu_buffer::create(device, description, initial_data);
}

AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_vertex_layout::create(device);
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* device, agpu_vertex_layout* layout)
{
    if (!device)
        return nullptr;

    return agpu_vertex_binding::create(device, layout);
}

AGPU_EXPORT agpu_shader* agpuCreateShader(agpu_device* device, agpu_shader_type type)
{
    if (!device)
        return nullptr;
    return agpu_shader::create(device, type);
}

AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_shader_signature_builder::create(device);
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_pipeline_builder::create(device);
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator(agpu_device* device, agpu_command_list_type type, agpu_command_queue *commandQueue)
{
    if (!device)
        return nullptr;
    return agpu_command_allocator::create(device, type, commandQueue);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList(agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    if (!device)
        return nullptr;
    return agpu_command_list::create(device, type, allocator, initial_pipeline_state);
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
