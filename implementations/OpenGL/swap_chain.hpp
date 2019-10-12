#ifndef AGPU_SWAP_CHAIN_HPP
#define AGPU_SWAP_CHAIN_HPP

#include "device.hpp"
#include "framebuffer.hpp"

namespace AgpuGL
{

struct GLSwapChain : public agpu::swap_chain
{
public:
    GLSwapChain();
    ~GLSwapChain();

    static agpu::swap_chain_ref create(const agpu::device_ref &device, const agpu::command_queue_ref &commandQueue, agpu_swap_chain_create_info *create_info);

    virtual agpu::framebuffer_ptr getCurrentBackBuffer() override;
    virtual agpu_size getCurrentBackBufferIndex() override;
    virtual agpu_size getFramebufferCount() override;

    virtual agpu_error swapBuffers() override;

	virtual agpu_error setOverlayPosition(agpu_int x, agpu_int y) override;

public:
    agpu::device_ref device;
    agpu_pointer window;
    agpu_uint width;
    agpu_uint height;
    agpu_uint backBufferIndex;
    std::vector<agpu::framebuffer_ref> framebuffers;
    agpu::command_queue_ref commandQueue;
};

} // End of namespace AgpuGL

#endif //AGPU_SWAP_CHAIN_HPP
