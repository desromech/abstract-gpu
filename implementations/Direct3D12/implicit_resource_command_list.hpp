#ifndef AGPU_D3D12_IMPLICIT_RESOURCE_COMMAND_LIST_HPP
#define AGPU_D3D12_IMPLICIT_RESOURCE_COMMAND_LIST_HPP

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <d3d12.h>
#include <wrl.h>
#include <mutex>
#include <algorithm>
#include "D3D12MemAlloc.h"
#include "common.hpp"
#include "../Common/utility.hpp"

namespace AgpuD3D12
{
using AgpuCommon::alignedTo;
using AgpuCommon::nextPowerOfTwo;
using Microsoft::WRL::ComPtr;
class ADXDevice;

class ADXImplicitResourceSetupCommandList
{
public:
    ADXImplicitResourceSetupCommandList(ADXDevice &cdevice);
    ~ADXImplicitResourceSetupCommandList();

    bool initializeWithQueue(const ComPtr<ID3D12CommandQueue> &aCommandQueue);

    bool transitionBufferUsageMode(const ComPtr<ID3D12Resource> &buffer, agpu_memory_heap_type heapType, agpu_buffer_usage_mask sourceUsage, agpu_buffer_usage_mask destinationUsage);
    bool transitionTextureUsageMode(const ComPtr<ID3D12Resource> &texture, agpu_memory_heap_type heapType, agpu_texture_usage_mode_mask sourceUsage, agpu_texture_usage_mode_mask destinationUsage, UINT subresourceIndex);

    bool setupCommandBuffer();
    bool submitCommandBuffer();
    bool waitForCompletion();

    inline bool submitCommandBufferAndWait()
    {
        return submitCommandBuffer() && waitForCompletion();
    }

    std::mutex mutex;
    ComPtr<ID3D12CommandQueue> commandQueue;

protected:
    agpu_error createStagingBuffer(agpu_memory_heap_type heapType, size_t allocationSize, D3D12_RESOURCE_STATES initialState, ID3D12Resource **outBuffer, D3D12MA::Allocation **outAllocation, void **mappedPointer);

    ADXDevice &device;

    HANDLE fenceEvent;
    ComPtr<ID3D12Fence> fence;
    UINT64 fenceValue;

    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> commandList;
};

template<agpu_memory_heap_type HT, D3D12_RESOURCE_STATES SBS, size_t IC>
class ADXImplicitResourceStagingCommandList : public ADXImplicitResourceSetupCommandList
{
public:
    static constexpr agpu_memory_heap_type MemoryHeapType = HT;
    static constexpr size_t InitialCapacity = IC;
    static constexpr D3D12_RESOURCE_STATES StagingBufferResourceStates = SBS;

    ADXImplicitResourceStagingCommandList(ADXDevice &cdevice)
        : ADXImplicitResourceSetupCommandList(cdevice),
        currentStagingBufferSize(0),
        currentStagingBufferPointer(nullptr),
        stagingBufferCapacity(0),
        stagingBufferBasePointer(nullptr)
    {}

    void ensureValidCPUStagingBuffer(size_t requiredSize, size_t requiredAlignment)
    {
        if(stagingBufferCapacity < requiredSize)
        {
            stagingBufferCapacity = nextPowerOfTwo(requiredSize);
            stagingBufferCapacity = std::max(stagingBufferCapacity, size_t(InitialCapacity));
            if(buffer)
            {
                buffer.Reset();
                bufferAllocation.Reset();
                stagingBufferBasePointer = nullptr;
            }

            createStagingBuffer(MemoryHeapType, stagingBufferCapacity, StagingBufferResourceStates, &buffer, &bufferAllocation, reinterpret_cast<void**> (&stagingBufferBasePointer));
        }

        currentStagingBufferSize = requiredSize;
        currentStagingBufferPointer = stagingBufferBasePointer;
    }

    bool lockBuffer()
    {
        if (stagingBufferBasePointer)
            return true;

        D3D12_RANGE range = {};
        range.End = currentStagingBufferSize;

        buffer->Map(0, &range, (void**)&stagingBufferBasePointer);
        currentStagingBufferPointer = stagingBufferBasePointer;

        return true;
    }

    bool unlockBuffer()
    {
        if (!stagingBufferBasePointer)
            return true;

        buffer->Unmap(0, nullptr);
        stagingBufferBasePointer = nullptr;
        return true;
    }

    bool uploadBufferData(const ComPtr<ID3D12Resource> &destBuffer, size_t offset, size_t size)
    {
        commandList->CopyBufferRegion(destBuffer.Get(), offset, buffer.Get(), 0, size);
        return true;
    }

    bool uploadBufferDataToImage(const ComPtr<ID3D12Resource> &destTexture, D3D12_TEXTURE_COPY_LOCATION copySourceLocation, D3D12_TEXTURE_COPY_LOCATION copyDestinationLocation)
    {
        copySourceLocation.pResource = buffer.Get();
        copyDestinationLocation.pResource = destTexture.Get();
        commandList->CopyTextureRegion(&copyDestinationLocation, 0, 0, 0, &copySourceLocation, nullptr);
        return true;
    }

    bool readbackImageDataToBuffer(D3D12_TEXTURE_COPY_LOCATION copyDestinationLocation, const ComPtr<ID3D12Resource>& sourceTexture, D3D12_TEXTURE_COPY_LOCATION copySourceLocation)
    {
        copyDestinationLocation.pResource = buffer.Get();
        copySourceLocation.pResource = sourceTexture.Get();
        commandList->CopyTextureRegion(&copyDestinationLocation, 0, 0, 0, &copySourceLocation, nullptr);
        return true;
    }

    bool readbackBufferData(const ComPtr<ID3D12Resource> &sourceBuffer, size_t offset, size_t size)
    {
        commandList->CopyBufferRegion(buffer.Get(), 0, sourceBuffer.Get(), offset, size);
        return true;
    }

public:
    size_t currentStagingBufferSize;
    void *currentStagingBufferPointer;

private:

    size_t stagingBufferCapacity;
    uint8_t *stagingBufferBasePointer;

    ComPtr<ID3D12Resource> buffer;
    ComPtr<D3D12MA::Allocation> bufferAllocation;
};

typedef ADXImplicitResourceStagingCommandList<AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE, D3D12_RESOURCE_STATE_GENERIC_READ, 2048*2048*4> ADXImplicitResourceUploadCommandList;
typedef ADXImplicitResourceStagingCommandList<AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST, D3D12_RESOURCE_STATE_COPY_DEST, 2048*2048*4> ADXImplicitResourceReadbackCommandList;

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_IMPLICIT_RESOURCE_COMMAND_LIST_HPP
