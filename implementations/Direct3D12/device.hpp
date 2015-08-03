#ifndef _AGPU_DEVICE_HPP_
#define _AGPU_DEVICE_HPP_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <string>
#include <wrl.h>

#include "object.hpp"

using Microsoft::WRL::ComPtr;
const int MaxFrameCount = 3; // Triple buffering.

/**
* Agpu D3D12 device
*/
struct _agpu_device : public Object<_agpu_device>
{
public:
    _agpu_device();

    void lostReferences();

    static agpu_device *open(agpu_device_open_info* openInfo);
    bool initialize(agpu_device_open_info* openInfo);

    agpu_error swapBuffers();
    agpu_error waitForPreviousFrame();

    HWND window;

    agpu_command_queue *defaultCommandQueue;

public:

    // Device objects
    ComPtr<IDXGISwapChain3> swapChain;
    ComPtr<ID3D12Device> d3dDevice;
    ComPtr<ID3D12Resource> mainFrameBufferTargets[MaxFrameCount];
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12DescriptorHeap> renderTargetViewHeap;
    UINT renderTargetViewDescriptorSize;

    int frameCount;
    int windowWidth, windowHeight;

    // Frame synchronization
    int frameIndex;
    HANDLE frameFenceEvent;
    ComPtr<ID3D12Fence> frameFence;
    UINT64 frameFenceValue;

private:
    bool getWindowSize();
};

#endif //_AGPU_DEVICE_HPP_

