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

    agpu_command_queue *defaultCommandQueue;

    agpu_int getMultiSampleQualityLevels(agpu_uint sample_count);

public:
    agpu_error withTransferQueue(std::function<agpu_error(const ComPtr<ID3D12CommandQueue> &)> function);
    agpu_error withTransferQueueAndCommandList(std::function<agpu_error(const ComPtr<ID3D12CommandQueue> &, const ComPtr<ID3D12GraphicsCommandList> &list)> function);
    
    agpu_error waitForMemoryTransfer();

    // Device objects
    ComPtr<ID3D12Device> d3dDevice;

    UINT renderTargetViewDescriptorSize;

    // Some states
    bool isOpened;

    // Extra settings.
    bool isDebugEnabled;

private:
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

