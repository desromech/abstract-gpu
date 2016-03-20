#include "swap_chain.hpp"
#include "texture_format.hpp"

_agpu_swap_chain::_agpu_swap_chain(agpu_device *device)
    : device(device)
{
    surface = nullptr;
    graphicsQueue = nullptr;
    presentationQueue = nullptr;
}

void _agpu_swap_chain::lostReferences()
{
    if(surface)
        vkDestroySurfaceKHR(device->vulkanInstance, surface, nullptr);
    if (graphicsQueue)
        graphicsQueue->release();
    if (presentationQueue)
        presentationQueue->release();
}

_agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_command_queue* graphicsCommandQueue, agpu_swap_chain_create_info *createInfo)
{
    VkSurfaceKHR surface;
    if (!graphicsCommandQueue || !createInfo)
        return nullptr;

#ifdef _WIN32
    if (!createInfo->window)
        return nullptr;
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    memset(&surfaceCreateInfo, 0, sizeof(surfaceCreateInfo));
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    surfaceCreateInfo.hwnd = (HWND)createInfo->window;

    auto error = vkCreateWin32SurfaceKHR(device->vulkanInstance, &surfaceCreateInfo, nullptr, &surface);
#endif
    if (error)
    {
        printError("Failed to create the swap chain surface\n");
        return false;
    }

    agpu_command_queue *presentationQueue = graphicsCommandQueue;
    if (!graphicsCommandQueue->supportsPresentingSurface(surface))
    {
        // TODO: Find a presentation queue.
        vkDestroySurfaceKHR(device->vulkanInstance, surface, nullptr);
        printError("Surface presentation in different queue is not yet supported.\n");
        return false;
    }

    // Create the swap chain object.
    auto swapChain = new agpu_swap_chain(device);
    swapChain->surface = surface;
    swapChain->graphicsQueue = graphicsCommandQueue;
    swapChain->presentationQueue = presentationQueue;
    graphicsCommandQueue->retain();
    presentationQueue->retain();

    // Initialize the rest of the swap chain.
    if (!swapChain->initialize(createInfo))
    {
        swapChain->release();
        return nullptr;
    }

    return swapChain;
}

bool _agpu_swap_chain::initialize(agpu_swap_chain_create_info *createInfo)
{
    return true;
}
