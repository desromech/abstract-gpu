#ifndef AGPU_SWAP_CHAIN_HPP
#define AGPU_SWAP_CHAIN_HPP

#include "device.hpp"
#include "command_queue.hpp"

/**
 * AGPU vulkan swap chain
 */
struct _agpu_swap_chain : public Object<_agpu_swap_chain>
{
public:
    _agpu_swap_chain(agpu_device *device);
    void lostReferences();

    static _agpu_swap_chain *create(agpu_device *device, agpu_command_queue* graphicsCommandQueue, agpu_swap_chain_create_info *createInfo);
    bool initialize(agpu_swap_chain_create_info *createInfo);

    agpu_error swapBuffers();
    agpu_framebuffer *getCurrentBackBuffer();

    agpu_device *device;
    VkSurfaceKHR surface;
    agpu_command_queue* graphicsQueue;
    agpu_command_queue* presentationQueue;

    agpu_uint swapChainWidth;
    agpu_uint swapChainHeight;
    VkFormat format;
    VkColorSpaceKHR colorSpace;

    VkSwapchainKHR handle;
    std::vector<agpu_framebuffer*> framebuffers;
};

#endif //AGPU_SWAP_CHAIN_HPP
