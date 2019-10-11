#include "device.hpp"
#include "common_commands.hpp"
#include "constants.hpp"

namespace AgpuD3D12
{

ADXImplicitResourceSetupCommandList::ADXImplicitResourceSetupCommandList(ADXDevice &cdevice)
    : device(cdevice), fenceEvent(NULL), fenceValue(1)
{
}

ADXImplicitResourceSetupCommandList::~ADXImplicitResourceSetupCommandList()
{
    if(fenceEvent)
        CloseHandle(fenceEvent);
}

bool ADXImplicitResourceSetupCommandList::initializeWithQueue(const ComPtr<ID3D12CommandQueue> &aCommandQueue)
{
	this->commandQueue = aCommandQueue;

    // Create the command allocator and the command list.
    {
        // Create the transfer command allocator.
        if (FAILED(device.d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator))))
            return false;

        // Create the transfer command list.
        if (FAILED(device.d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList))))
            return false;

        if (FAILED(commandList->Close()))
            return false;
    }

    // Create the fence
    if (FAILED(device.d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence))))
        return false;
    fenceValue = 1;

    // Create an event handle to use for frame synchronization.
    fenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (fenceEvent == nullptr)
        return false;

    return true;
}

bool ADXImplicitResourceSetupCommandList::setupCommandBuffer()
{
    if(FAILED(commandAllocator->Reset()))
        return false;
    if(FAILED(commandList->Reset(commandAllocator.Get(), nullptr)))
        return false;
    return true;
}

bool ADXImplicitResourceSetupCommandList::waitForCompletion()
{
    // Signal the fence.
    auto expectedFenceValue = fenceValue;
    if(FAILED(commandQueue->Signal(fence.Get(), expectedFenceValue)))
        return false;
    ++fenceValue;

    // Wait until previous frame is finished.
    if (fence->GetCompletedValue() < expectedFenceValue)
    {
        ERROR_IF_FAILED(fence->SetEventOnCompletion(expectedFenceValue, fenceEvent));
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    return true;
}

bool ADXImplicitResourceSetupCommandList::transitionBufferUsageMode(const ComPtr<ID3D12Resource> &buffer, agpu_memory_heap_type heapType, agpu_buffer_usage_mask sourceUsage, agpu_buffer_usage_mask destinationUsage)
{
    if(sourceUsage == destinationUsage)
        return true;

	auto sourceState = mapBufferUsageToResourceState(heapType, sourceUsage);
	auto destinationState = mapBufferUsageToResourceState(heapType, destinationUsage);
	if (sourceState == destinationState)
		return true;

    auto barrier = resourceTransitionBarrier(buffer.Get(), sourceState, destinationState);
    commandList->ResourceBarrier(1, &barrier);
    return true;
}

bool ADXImplicitResourceSetupCommandList::transitionTextureUsageMode(const ComPtr<ID3D12Resource> &texture, agpu_memory_heap_type heapType, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destinationUsage, UINT subresourceIndex)
{
    if(sourceUsage == destinationUsage)
        return true;

    auto sourceState = mapTextureUsageToResourceState(heapType, sourceUsage);
	auto destinationState = mapTextureUsageToResourceState(heapType, destinationUsage);
	if (sourceState == destinationState)
		return true;

    auto barrier = resourceTransitionBarrier(texture.Get(), sourceState, destinationState, subresourceIndex);
    commandList->ResourceBarrier(1, &barrier);
    return true;
}

bool ADXImplicitResourceSetupCommandList::submitCommandBuffer()
{
	commandList->Close();

	ID3D12CommandList* lists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(1, lists);
    commandQueue->Signal(fence.Get(), fenceValue);
    return true;
}

agpu_error ADXImplicitResourceSetupCommandList::createStagingBuffer(agpu_memory_heap_type heapType, size_t allocationSize, D3D12_RESOURCE_STATES initialState, ID3D12Resource **outBuffer, D3D12MA::Allocation **outAllocation, void **mappedPointer)
{
    // The resource description.
    D3D12_RESOURCE_DESC desc = {};
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    desc.Alignment = 0;
    desc.Width = allocationSize;
    desc.Height = 1;
    desc.DepthOrArraySize = 1;
    desc.MipLevels = 1;
    desc.Format = DXGI_FORMAT_UNKNOWN;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12MA::ALLOCATION_DESC allocationDesc = {};
    allocationDesc.HeapType = mapHeapType(heapType);

    if(FAILED(device.memoryAllocator->CreateResource(&allocationDesc, &desc, initialState, NULL, outAllocation, IID_PPV_ARGS(outBuffer))))
        return AGPU_ERROR;

    auto resource = *outBuffer;
    if(FAILED(resource->Map(0, nullptr, mappedPointer)))
        return AGPU_ERROR;

    return AGPU_OK;
}


} // End of namespace AgpuD3D12
