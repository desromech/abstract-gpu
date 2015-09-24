#include "framebuffer.hpp"
#include "swap_chain.hpp"

_agpu_swap_chain::_agpu_swap_chain()
    : doublebuffer(false), backBufferIndex(0)
{
    buffers[0] = buffers[1] = nullptr;
}

void _agpu_swap_chain::lostReferences()
{
    for(int i = 0; i < 2; ++i)
    {
        auto buffer = buffers[i];
        if(buffer)
            buffer->release();
    }
}

agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_swap_chain_create_info *create_info)
{
    auto chain = new agpu_swap_chain();
    chain->device = device;
    chain->window = create_info->window;
    chain->doublebuffer = create_info->doublebuffer;
    return chain;
}

agpu_error _agpu_swap_chain::swapBuffers()
{
    device->onMainContextBlocking([&](){
        // Use the window.
        auto res = device->mainContext.makeCurrentWithWindow(window);

        // TODO: Blit the framebuffer.
        glClearColor(0,1,0,0);
        glClear(GL_COLOR_BUFFER_BIT);
        glFinish();

        // Swap the window buffers.
        device->mainContext.swapBuffersOfWindow(window);

        // Restore the main window.
        device->mainContext.makeCurrent();
    });

    if(doublebuffer)
        backBufferIndex = (backBufferIndex + 1) % 2;
    return AGPU_OK;
}

agpu_framebuffer* _agpu_swap_chain::getCurrentBackBuffer ()
{
    return buffers[backBufferIndex];
}

// The exported C interface
AGPU_EXPORT agpu_error agpuAddSwapChainReference ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain);
    return swap_chain->retain();
}

AGPU_EXPORT agpu_error agpuReleaseSwapChain ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain);
    return swap_chain->release();
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_swap_chain* swap_chain )
{
    if(!swap_chain)
        return nullptr;
    return swap_chain->getCurrentBackBuffer();
}

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain);
    return swap_chain->swapBuffers();
}
