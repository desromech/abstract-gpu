#ifndef AGPU_D3D12_SWAP_CHAIN_HPP
#define AGPU_D3D12_SWAP_CHAIN_HPP

#include <vector>
#include "device.hpp"

struct _agpu_swap_chain: public Object<_agpu_swap_chain>
{
public:
    static const int MaxFrameCount = 3; // Triple buffering.

    _agpu_swap_chain();

    void lostReferences();

    static _agpu_swap_chain *create(agpu_device *device, agpu_command_queue *queue, agpu_swap_chain_create_info *createInfo);

    agpu_framebuffer* getCurrentBackBuffer();
    agpu_error swapBuffers();

public:
    agpu_device *device;

    HWND window;

    // Frame buffers
    ComPtr<IDXGISwapChain3> swapChain;
    agpu_framebuffer *framebuffer[MaxFrameCount];

    int frameIndex;
    int frameCount;
    int windowWidth, windowHeight;
};

#endif //AGPU_D3D12_VERTEX_LAYOUT_HPP
