#ifndef AGPU_METAL_SWAP_CHAIN_HPP
#define AGPU_METAL_SWAP_CHAIN_HPP

#include "device.hpp"
#include <vector>
#import <QuartzCore/CAMetalLayer.h>
#import <AppKit/AppKit.h>

namespace AgpuMetal
{
class AMtlSwapChain;
};

@interface AGPUSwapChainView : NSView
{
    AgpuMetal::AMtlSwapChain *swapChain;
}
-(AGPUSwapChainView*) initWithSwapChain: (AgpuMetal::AMtlSwapChain*)swapChain;

@end

namespace AgpuMetal
{
    
class AMtlSwapChain : public agpu::swap_chain
{
public:
    AMtlSwapChain(const agpu::device_ref &device);
    ~AMtlSwapChain();

    static agpu::swap_chain_ref create(const agpu::device_ref &device, const agpu::command_queue_ref &presentQueue, agpu_swap_chain_create_info *createInfo);

    virtual agpu_error swapBuffers() override;
    virtual agpu::framebuffer_ptr getCurrentBackBuffer() override;
    virtual agpu_size getCurrentBackBufferIndex() override;
    virtual agpu_size getFramebufferCount() override;

    agpu::device_ref device;
    NSWindow *window;
    CAMetalLayer *metalLayer;
    AGPUSwapChainView *view;
    agpu::command_queue_ref presentQueue;
    agpu_swap_chain_create_info swapChainInfo;
    std::vector<agpu::framebuffer_ref> framebuffers;
    std::vector<id<MTLCommandBuffer> > presentCommands;

    size_t currentFramebufferIndex;
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_SWAP_CHAIN_HPP
