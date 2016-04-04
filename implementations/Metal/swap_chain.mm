#include "swap_chain.hpp"

_agpu_swap_chain::_agpu_swap_chain(agpu_device *device)
{
}

void _agpu_swap_chain::lostReferences()
{
}

agpu_swap_chain *_agpu_swap_chain::create(agpu_device *device, agpu_swap_chain_create_info *createInfo)
{
    if(!createInfo || !createInfo->window)
        return nullptr;

    std::unique_ptr<agpu_swap_chain> result(new agpu_swap_chain(device));
    auto window = (NSWindow*)createInfo->window;
    result->window = window;
    return result.release();
}

agpu_error _agpu_swap_chain::swapBuffers (  )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_framebuffer* _agpu_swap_chain::getCurrentBackBuffer (  )
{
    return nullptr;
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

AGPU_EXPORT agpu_error agpuSwapBuffers ( agpu_swap_chain* swap_chain )
{
    CHECK_POINTER(swap_chain)
    return swap_chain->swapBuffers();
}

AGPU_EXPORT agpu_framebuffer* agpuGetCurrentBackBuffer ( agpu_swap_chain* swap_chain )
{
    if(!swap_chain)
        return nullptr;
    return swap_chain->getCurrentBackBuffer();
}
