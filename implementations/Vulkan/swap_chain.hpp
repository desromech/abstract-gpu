#ifndef AGPU_SWAP_CHAIN_HPP
#define AGPU_SWAP_CHAIN_HPP

#include "device.hpp"
#include "command_queue.hpp"
#include "../Common/overlay_window.hpp"

namespace AgpuVulkan
{

/**
 * AGPU vulkan swap chain
 */
struct AVkSwapChain : public agpu::swap_chain
{
public:
    AVkSwapChain(const agpu::device_ref &device);
    ~AVkSwapChain();

    static agpu::swap_chain_ref create(const agpu::device_ref &device, const agpu::command_queue_ref &graphicsCommandQueue, agpu_swap_chain_create_info *createInfo);
    bool initialize(agpu_swap_chain_create_info *createInfo);

    virtual agpu_error swapBuffers() override;
    virtual agpu::framebuffer_ptr getCurrentBackBuffer() override;
    virtual agpu_size getCurrentBackBufferIndex() override;
    virtual agpu_size getFramebufferCount() override;

    virtual agpu_error setOverlayPosition(agpu_int x, agpu_int y) override;

    agpu::device_ref device;
    VkSurfaceKHR surface;
    agpu::command_queue_ref graphicsQueue;
    agpu::command_queue_ref presentationQueue;

    agpu_uint swapChainWidth;
    agpu_uint swapChainHeight;
    agpu_texture_format agpuFormat;
    VkFormat format;
    VkColorSpaceKHR colorSpace;

    VkSwapchainKHR handle;
    std::vector<VkSemaphore> semaphores;
    std::vector<agpu::framebuffer_ref> framebuffers;

    uint32_t imageCount;
    uint32_t currentBackBufferIndex;
    uint32_t currentSemaphoreIndex;
    AgpuCommon::OverlaySwapChainWindowPtr overlayWindow;
private:
    agpu_error getNextBackBufferIndex();
};

} // End of namespace AgpuVulkan

#endif //AGPU_SWAP_CHAIN_HPP
