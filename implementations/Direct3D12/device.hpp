#ifndef _AGPU_DEVICE_HPP_
#define _AGPU_DEVICE_HPP_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <D3Dcompiler.h>
#include <string>
#include <mutex>
#include <functional>
#include <wrl.h>

#include "object.hpp"

using Microsoft::WRL::ComPtr;
const int MaxFrameCount = 3; // Triple buffering.
const int ShaderTypeCount = 6;

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
    agpu_error withTransferQueue(std::function<agpu_error(const ComPtr<ID3D12CommandQueue> &)> function);
    agpu_error withTransferQueueAndCommandList(std::function<agpu_error(const ComPtr<ID3D12CommandQueue> &, const ComPtr<ID3D12GraphicsCommandList> &list)> function);
    
    agpu_error waitForMemoryTransfer();

    // Device objects
    ComPtr<IDXGISwapChain3> swapChain;
    ComPtr<ID3D12Device> d3dDevice;
    ComPtr<ID3D12Resource> mainFrameBufferTargets[MaxFrameCount];

    // Descriptor heaprs.
    ComPtr<ID3D12DescriptorHeap> renderTargetViewHeap;
    ComPtr<ID3D12DescriptorHeap> shaderResourcesViewHeaps[4];
    ComPtr<ID3D12DescriptorHeap> samplersViewHeaps[4];

    UINT renderTargetViewDescriptorSize;
    UINT shaderResourceViewDescriptorSize;

    int frameCount;
    int windowWidth, windowHeight;

    // Frame synchronization
    int frameIndex;
    HANDLE frameFenceEvent;
    ComPtr<ID3D12Fence> frameFence;
    UINT64 frameFenceValue;

    // Some states
    bool isOpened;

    // Extra settings.
    bool isDebugEnabled;

    // Root signatures
    ComPtr<ID3D12RootSignature> graphicsRootSignature;

private:
    bool getWindowSize();
    agpu_error createGraphicsRootSignature();

    // For immediate and blocking data transferring.
    ComPtr<ID3D12CommandAllocator> transferCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> transferCommandList;

    std::mutex transferMutex;
    ComPtr<ID3D12CommandQueue> transferCommandQueue;
    HANDLE transferFenceEvent;
    ComPtr<ID3D12Fence> transferFence;
    UINT64 transferFenceValue;

};

#endif //_AGPU_DEVICE_HPP_

