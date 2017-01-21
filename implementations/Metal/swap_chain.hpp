#ifndef AGPU_METAL_SWAP_CHAIN_HPP
#define AGPU_METAL_SWAP_CHAIN_HPP

#include "device.hpp"
#include <vector>
#import <QuartzCore/CAMetalLayer.h>
#import <AppKit/AppKit.h>

@interface AGPUSwapChainView : NSView
{
    _agpu_swap_chain *swapChain;
}
-(AGPUSwapChainView*) initWithSwapChain: (agpu_swap_chain*)swapChain;

@end

struct _agpu_swap_chain : public Object<_agpu_swap_chain>
{
public:
    _agpu_swap_chain(agpu_device *device);
    void lostReferences();

    static agpu_swap_chain *create(agpu_device *device, agpu_command_queue *presentQueue, agpu_swap_chain_create_info *createInfo);

    agpu_error swapBuffers (  );
    agpu_framebuffer* getCurrentBackBuffer (  );
    agpu_size getCurrentBackBufferIndex ( );
    agpu_size getFramebufferCount ( );

    agpu_device *device;
    NSWindow *window;
    CAMetalLayer *metalLayer;
    AGPUSwapChainView *view;
    agpu_command_queue *presentQueue;
    agpu_swap_chain_create_info swapChainInfo;
    std::vector<agpu_framebuffer*> framebuffers;
    std::vector<id<MTLCommandBuffer> > presentCommands;

    size_t currentFramebufferIndex;
};

#endif //AGPU_METAL_SWAP_CHAIN_HPP
