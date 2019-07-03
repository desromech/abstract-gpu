#include <algorithm>
#include "swap_chain.hpp"
#include "texture.hpp"
#include "texture_format.hpp"
#include "framebuffer.hpp"

#ifdef __unix__
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>
#endif

namespace AgpuVulkan
{

AVkSwapChain::AVkSwapChain(const agpu::device_ref &device)
    : device(device)
{
    surface = VK_NULL_HANDLE;
    handle = VK_NULL_HANDLE;
    currentBackBufferIndex = 0;
}

AVkSwapChain::~AVkSwapChain()
{
    for(auto semaphore : semaphores)
    {
        if(semaphore)
            vkDestroySemaphore(deviceForVk->device, semaphore, nullptr);
    }

    if (handle)
        vkDestroySwapchainKHR(deviceForVk->device, handle, nullptr);
    if(surface)
        vkDestroySurfaceKHR(deviceForVk->vulkanInstance, surface, nullptr);
}

static VkResult createXlibSurface(const agpu::device_ref &device, const agpu::command_queue_ref &graphicsCommandQueue, agpu_swap_chain_create_info *createInfo, VkSurfaceKHR *surface)
{
#ifdef __unix__
    if(!deviceForVk->displayHandle)
        deviceForVk->displayHandle = XOpenDisplay(nullptr);

    if (!deviceForVk->displayHandle || !createInfo->window)
        return VK_INCOMPLETE;

    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo;
    memset(&surfaceCreateInfo, 0, sizeof(surfaceCreateInfo));
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.connection = XGetXCBConnection((Display*)deviceForVk->displayHandle);
    surfaceCreateInfo.window = (xcb_window_t)(uintptr_t)createInfo->window;

    return vkCreateXcbSurfaceKHR(deviceForVk->vulkanInstance, &surfaceCreateInfo, nullptr, surface);
#else
    return VK_INCOMPLETE;
#endif
}

static VkResult createXcbSurface(const agpu::device_ref &device, const agpu::command_queue_ref &graphicsCommandQueue, agpu_swap_chain_create_info *createInfo, VkSurfaceKHR *surface)
{
#ifdef __unix__
    if(!deviceForVk->displayHandle)
        deviceForVk->displayHandle = xcb_connect(nullptr, nullptr);

    if (!deviceForVk->displayHandle || !createInfo->window)
        return VK_INCOMPLETE;

    VkXcbSurfaceCreateInfoKHR surfaceCreateInfo;
    memset(&surfaceCreateInfo, 0, sizeof(surfaceCreateInfo));
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.connection = XGetXCBConnection((Display*)deviceForVk->displayHandle);
    surfaceCreateInfo.window = (xcb_window_t)(uintptr_t)createInfo->window;

    return vkCreateXcbSurfaceKHR(deviceForVk->vulkanInstance, &surfaceCreateInfo, nullptr, surface);
#else
    return VK_INCOMPLETE;
#endif
}

static VkResult createWin32Surface(const agpu::device_ref &device, const agpu::command_queue_ref &graphicsCommandQueue, agpu_swap_chain_create_info *createInfo, VkSurfaceKHR *surface)
{
#ifdef _WIN32
    if (!createInfo->window)
        return VK_INCOMPLETE;
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    memset(&surfaceCreateInfo, 0, sizeof(surfaceCreateInfo));
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = (HWND)createInfo->window;

    return vkCreateWin32SurfaceKHR(deviceForVk->vulkanInstance, &surfaceCreateInfo, nullptr, surface);
#else
    return VK_INCOMPLETE;
#endif
}

static VkResult createDisplaySurface(const agpu::device_ref &device, const agpu::command_queue_ref &graphicsCommandQueue, agpu_swap_chain_create_info *createInfo, VkSurfaceKHR *surface)
{
    return VK_INCOMPLETE;
}

static VkResult createDefaultSurface(const agpu::device_ref &device, const agpu::command_queue_ref &graphicsCommandQueue, agpu_swap_chain_create_info *createInfo, VkSurfaceKHR *surface)
{
#if defined(_WIN32)
    return createWin32Surface(device, graphicsCommandQueue, createInfo, surface);
#elif defined(__unix__)
    return createXlibSurface(device, graphicsCommandQueue, createInfo, surface);
#else
#error unsupported platform
#endif
}

agpu::swap_chain_ref AVkSwapChain::create(const agpu::device_ref &device, const agpu::command_queue_ref &graphicsCommandQueue, agpu_swap_chain_create_info *createInfo)
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    if (!graphicsCommandQueue || !createInfo)
        return agpu::swap_chain_ref();

    VkResult error = VK_ERROR_FEATURE_NOT_PRESENT;
    if(!createInfo->window_system_name)
        error = createDefaultSurface(device, graphicsCommandQueue, createInfo, &surface);
    else if(!strcmp(createInfo->window_system_name, "xlib"))
        error = createXlibSurface(device, graphicsCommandQueue, createInfo, &surface);
    else if(!strcmp(createInfo->window_system_name, "xcb"))
        error = createXcbSurface(device, graphicsCommandQueue, createInfo, &surface);
    else if(!strcmp(createInfo->window_system_name, "win32"))
        error = createWin32Surface(device, graphicsCommandQueue, createInfo, &surface);
    else if(!strcmp(createInfo->window_system_name, "display"))
        error = createDisplaySurface(device, graphicsCommandQueue, createInfo, &surface);

    if(error || surface == VK_NULL_HANDLE)
    {
        printError("Failed to create the swap chain surface\n");
        return agpu::swap_chain_ref();
    }

    auto presentationQueue = graphicsCommandQueue;
    if (!graphicsCommandQueue.as<AVkCommandQueue> ()->supportsPresentingSurface(surface))
    {
        // TODO: Find a presentation queue.
        vkDestroySurfaceKHR(deviceForVk->vulkanInstance, surface, nullptr);
        printError("Surface presentation in different queue is not yet supported.\n");
        return agpu::swap_chain_ref();
    }

    uint32_t formatCount = 0;
    error = deviceForVk->fpGetPhysicalDeviceSurfaceFormatsKHR(deviceForVk->physicalDevice, surface, &formatCount, nullptr);
    if (error)
    {
        vkDestroySurfaceKHR(deviceForVk->vulkanInstance, surface, nullptr);
        return agpu::swap_chain_ref();
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
    error = deviceForVk->fpGetPhysicalDeviceSurfaceFormatsKHR(deviceForVk->physicalDevice, surface, &formatCount, &surfaceFormats[0]);
    if (error)
    {
        vkDestroySurfaceKHR(deviceForVk->vulkanInstance, surface, nullptr);
        return agpu::swap_chain_ref();
    }

    // Create the swap chain object.
    auto result = agpu::makeObject<AVkSwapChain> (device);
    auto swapChain = result.as<AVkSwapChain> ();
    swapChain->surface = surface;
    swapChain->graphicsQueue = graphicsCommandQueue;
    swapChain->presentationQueue = presentationQueue;

    // Set the format.
    agpu_texture_format actualFormat = createInfo->colorbuffer_format;
    if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
    {
        swapChain->format = mapTextureFormat(createInfo->colorbuffer_format);
        if (swapChain->format == VK_FORMAT_UNDEFINED)
        {
            swapChain->format = VK_FORMAT_B8G8R8A8_UNORM;
            actualFormat = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
        }
        swapChain->colorSpace = surfaceFormats[0].colorSpace;
    }
    else
    {
        assert(formatCount >= 1);

        // Start selecting the first format.
        swapChain->format = surfaceFormats[0].format;
        swapChain->colorSpace = surfaceFormats[0].colorSpace;
        actualFormat = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM;
        if(swapChain->format == VK_FORMAT_B8G8R8A8_SRGB)
            actualFormat = AGPU_TEXTURE_FORMAT_B8G8R8A8_UNORM_SRGB;

        // Try to select the expected format.
        auto wantedFormat = mapTextureFormat(createInfo->colorbuffer_format);
        for(size_t i = 0; i < formatCount; ++i)
        {
            auto &format = surfaceFormats[i];
            if(format.format == wantedFormat)
            {
                swapChain->format = format.format;
                swapChain->colorSpace = format.colorSpace;
                actualFormat = createInfo->colorbuffer_format;
                break;
            }
        }

    }
    swapChain->agpuFormat = actualFormat;

    // Initialize the rest of the swap chain.
    if (!swapChain->initialize(createInfo))
        return agpu::swap_chain_ref();

    return result;
}

bool AVkSwapChain::initialize(agpu_swap_chain_create_info *createInfo)
{
    swapChainWidth = createInfo->width;
    swapChainHeight = createInfo->height;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    auto error = deviceForVk->fpGetPhysicalDeviceSurfaceCapabilitiesKHR(deviceForVk->physicalDevice, surface, &surfaceCapabilities);
    if (error)
        return false;

    uint32_t presentModeCount;
    error = deviceForVk->fpGetPhysicalDeviceSurfacePresentModesKHR(deviceForVk->physicalDevice, surface, &presentModeCount, nullptr);
    if (error)
        return false;

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    error = deviceForVk->fpGetPhysicalDeviceSurfacePresentModesKHR(deviceForVk->physicalDevice, surface, &presentModeCount, &presentModes[0]);
    if (error)
        return false;

    VkExtent2D swapchainExtent;
    if (surfaceCapabilities.currentExtent.width == (uint32_t)-1)
    {
        swapchainExtent.width = swapChainWidth;
        swapchainExtent.height = swapChainHeight;
    }
    else
    {
        swapchainExtent = surfaceCapabilities.currentExtent;
        createInfo->width = swapChainWidth = swapchainExtent.width;
        createInfo->height = swapChainHeight = swapchainExtent.height;
    }

    VkPresentModeKHR swapchainPresentMode = presentModes[0];
    for (size_t i = 0; i < presentModeCount; i++) {
        if (presentModes[i] == VK_PRESENT_MODE_FIFO_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
        }

        if (presentModes[i] == VK_PRESENT_MODE_FIFO_RELAXED_KHR) {
            swapchainPresentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;
            break;
        }
    }

    uint32_t desiredNumberOfSwapchainImages = std::max(surfaceCapabilities.minImageCount, createInfo->buffer_count);
    if ((surfaceCapabilities.maxImageCount > 0) &&
        (desiredNumberOfSwapchainImages > surfaceCapabilities.maxImageCount))
        desiredNumberOfSwapchainImages = surfaceCapabilities.maxImageCount;
    VkSurfaceTransformFlagsKHR preTransform;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else {
        preTransform = surfaceCapabilities.currentTransform;
    }

    VkSwapchainCreateInfoKHR swapchainInfo;
    memset(&swapchainInfo, 0, sizeof(swapchainInfo));
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.surface = surface;
    swapchainInfo.minImageCount = desiredNumberOfSwapchainImages;
    swapchainInfo.imageFormat = format;
    swapchainInfo.imageColorSpace = colorSpace;
    swapchainInfo.imageExtent = swapchainExtent;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    swapchainInfo.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
    swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.presentMode = swapchainPresentMode;
    swapchainInfo.oldSwapchain = handle;

    error = deviceForVk->fpCreateSwapchainKHR(deviceForVk->device, &swapchainInfo, nullptr, &handle);
    if (error)
        return false;

    error = deviceForVk->fpGetSwapchainImagesKHR(deviceForVk->device, handle, &imageCount, nullptr);
    if (error)
        return false;

    std::vector<VkImage> swapChainImages(imageCount);
    error = deviceForVk->fpGetSwapchainImagesKHR(deviceForVk->device, handle, &imageCount, &swapChainImages[0]);
    if (error)
        return false;

    // Color buffers descriptions
    agpu_texture_description colorDesc = {};
    colorDesc.type = AGPU_TEXTURE_2D;
    colorDesc.width = createInfo->width;
    colorDesc.height = createInfo->height;
    colorDesc.depth = 1;
    colorDesc.layers = 1;
    colorDesc.format = agpuFormat;
    colorDesc.usage_modes = AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT;
    colorDesc.main_usage_mode = AGPU_TEXTURE_USAGE_COLOR_ATTACHMENT;
    colorDesc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    colorDesc.miplevels = 1;
    colorDesc.sample_count = 1;

    // Depth stencil buffer descriptions.
    agpu_texture_description depthStencilDesc = {};
    depthStencilDesc.type = AGPU_TEXTURE_2D;
    depthStencilDesc.width = createInfo->width;
    depthStencilDesc.height = createInfo->height;
    depthStencilDesc.depth = 1;
    depthStencilDesc.layers = 1;
    depthStencilDesc.format = createInfo->depth_stencil_format;
    depthStencilDesc.heap_type = AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL;
    depthStencilDesc.miplevels = 1;

    bool hasDepth = hasDepthComponent(createInfo->depth_stencil_format);
    bool hasStencil = hasStencilComponent(createInfo->depth_stencil_format);
    if (hasDepth)
        depthStencilDesc.usage_modes = agpu_texture_usage_mode_mask(depthStencilDesc.usage_modes | AGPU_TEXTURE_USAGE_DEPTH_ATTACHMENT);
    if (hasStencil)
        depthStencilDesc.usage_modes = agpu_texture_usage_mode_mask(depthStencilDesc.usage_modes | AGPU_TEXTURE_USAGE_STENCIL_ATTACHMENT);
    depthStencilDesc.main_usage_mode = depthStencilDesc.usage_modes;

    // Create the semaphores
    VkSemaphoreCreateInfo semaphoreInfo;
    memset(&semaphoreInfo, 0, sizeof(semaphoreInfo));
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphores.resize(imageCount, VK_NULL_HANDLE);
    currentSemaphoreIndex = 0;
    for (size_t i = 0; i < imageCount; ++i)
    {
        error = vkCreateSemaphore(deviceForVk->device, &semaphoreInfo, nullptr, &semaphores[i]);
        if(error)
            return false;
    }

    // Since the depth stencil buffer is used exclusively on the GPU, there is
    // no need to use triple buffering on it.
    agpu::texture_ref depthStencilBuffer;
    agpu::texture_view_ref depthStencilBufferView;
    if(hasDepth || hasStencil)
    {
        depthStencilBuffer = AVkTexture::create(device, &depthStencilDesc);
        if (!depthStencilBuffer)
            return false;

        // Get the depth stencil buffer view description.
        depthStencilBufferView = agpu::texture_view_ref(depthStencilBuffer->getOrCreateFullView());
    }

    VkClearColorValue clearColor;
    memset(&clearColor, 0, sizeof(clearColor));
    framebuffers.resize(imageCount);
    for (size_t i = 0; i < imageCount; ++i)
    {
        auto colorImage = swapChainImages[i];
        VkImageSubresourceRange range;
        memset(&range, 0, sizeof(range));
        range.layerCount = 1;
        range.levelCount = 1;
        if (!deviceForVk->clearImageWithColor(colorImage, range, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VkAccessFlagBits(0), &clearColor))
            return false;

        agpu::texture_ref colorBuffer;

        {
            colorBuffer = AVkTexture::createFromImage(device, &colorDesc, colorImage);
            if (!colorBuffer)
                return false;

            auto avkColorBuffer = colorBuffer.as<AVkTexture> ();
            avkColorBuffer->initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
            avkColorBuffer->initialLayoutAccessBits = VK_ACCESS_MEMORY_READ_BIT;
            avkColorBuffer->imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        auto colorBufferView = agpu::texture_view_ref(colorBuffer->getOrCreateFullView());
        auto framebuffer = AVkFramebuffer::create(device, swapChainWidth, swapChainHeight, 1, &colorBufferView, depthStencilBufferView);
        framebuffer.as<AVkFramebuffer> ()->swapChainFramebuffer = true;
        framebuffers[i] = framebuffer;

        // Release the references to the buffers.
        if (!framebuffer)
            return false;
    }

    if (!getNextBackBufferIndex())
        return false;

    return true;
}

bool AVkSwapChain::getNextBackBufferIndex()
{
    auto semaphore = semaphores[currentSemaphoreIndex];
    currentSemaphoreIndex = (currentSemaphoreIndex + 1) % semaphores.size();
    auto error = deviceForVk->fpAcquireNextImageKHR(deviceForVk->device, handle, UINT64_MAX, semaphore, VK_NULL_HANDLE, &currentBackBufferIndex);
    if (error == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // TODO: Recreate the swap chain.
        printf("Swap chain is out of date\n");
        return true;
    }
    else if (error == VK_SUBOPTIMAL_KHR)
    {
    }
    else if(error)
    {
        return false;
    }

    {
        VkSubmitInfo submit;
        memset(&submit, 0, sizeof(submit));
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &semaphore;

        VkPipelineStageFlags waitDstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
        submit.pWaitDstStageMask = &waitDstStageMask;

        vkQueueSubmit(graphicsQueue.as<AVkCommandQueue> ()->queue, 1, &submit, VK_NULL_HANDLE);
    }

    return true;
}

agpu_error AVkSwapChain::swapBuffers()
{
    // Present.
    VkPresentInfoKHR presentInfo;
    memset(&presentInfo, 0, sizeof(presentInfo));
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &handle;
    presentInfo.pImageIndices = &currentBackBufferIndex;
    auto error = deviceForVk->fpQueuePresentKHR(presentationQueue.as<AVkCommandQueue> ()->queue, &presentInfo);

    if (error == VK_ERROR_OUT_OF_DATE_KHR)
    {
        // TODO: Recreate the swap chain.
        printf("TODO: Out of date swap chain\n");
        return AGPU_OK;
    }
    else if (error == VK_SUBOPTIMAL_KHR)
    {
        printf("khronos suboptimal\n");
    }
    else if (error)
    {
        CONVERT_VULKAN_ERROR(error);
    }

    if (!getNextBackBufferIndex())
        return AGPU_ERROR;
    return AGPU_OK;
}

agpu::framebuffer_ptr AVkSwapChain::getCurrentBackBuffer()
{
    return framebuffers[currentBackBufferIndex].disownedNewRef();
}

agpu_size AVkSwapChain::getCurrentBackBufferIndex ( )
{
    return currentBackBufferIndex;
}

agpu_size AVkSwapChain::getFramebufferCount ( )
{
    return (agpu_size)framebuffers.size();
}

} // End of namespace AgpuVulkan
