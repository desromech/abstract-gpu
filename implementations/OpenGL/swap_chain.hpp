#ifndef AGPU_SWAP_CHAIN_HPP
#define AGPU_SWAP_CHAIN_HPP

#include "device.hpp"
#include "framebuffer.hpp"

struct _agpu_swap_chain : public Object<_agpu_swap_chain>
{
public:
    _agpu_swap_chain();

    void lostReferences();

    static agpu_swap_chain *create(agpu_device *device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info *create_info);
    agpu_framebuffer* getCurrentBackBuffer ();
    agpu_size getCurrentBackBufferIndex ();
    agpu_size getFramebufferCount ();

    agpu_error swapBuffers();

public:

    agpu_device *device;
    agpu_pointer window;
    agpu_uint width;
    agpu_uint height;
    agpu_uint backBufferIndex;
    std::vector<agpu_framebuffer*> framebuffers;
    agpu_command_queue* commandQueue;
};


#endif //AGPU_SWAP_CHAIN_HPP
