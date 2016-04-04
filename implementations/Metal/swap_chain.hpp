#ifndef AGPU_METAL_SWAP_CHAIN_HPP
#define AGPU_METAL_SWAP_CHAIN_HPP

#include "device.hpp"
#import <AppKit/AppKit.h>

@interface SwapChainWindowView : NSView
@property _agpu_swap_chain *swapChain;
@end

struct _agpu_swap_chain : public Object<_agpu_swap_chain>
{
public:
    _agpu_swap_chain(agpu_device *device);
    void lostReferences();

    static agpu_swap_chain *create(agpu_device *device, agpu_swap_chain_create_info *createInfo);

    agpu_error swapBuffers (  );
    agpu_framebuffer* getCurrentBackBuffer (  );

    agpu_device *device;
    NSWindow *window;
};

#endif //AGPU_METAL_SWAP_CHAIN_HPP
