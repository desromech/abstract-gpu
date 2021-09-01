#ifndef AGPU_METAL_IMPLICIT_RESOURCE_COMMAND_LIST_HPP
#define AGPU_METAL_IMPLICIT_RESOURCE_COMMAND_LIST_HPP

#include "common.hpp"
#include "../Common/utility.hpp"
#import <Metal/Metal.h>
#include <memory>
#include <mutex>
#include <algorithm>

namespace AgpuMetal
{
using AgpuCommon::alignedTo;
using AgpuCommon::nextPowerOfTwo;
class AMtlDevice;

class AMtlImplicitResourceSetupCommandList
{
public:
    AMtlImplicitResourceSetupCommandList(AMtlDevice &cdevice);
    ~AMtlImplicitResourceSetupCommandList();

	void destroy();
    
    bool setupCommandBuffer();
    bool submitCommandBuffer();

    AMtlDevice &device;

    std::mutex mutex;
    agpu::command_queue_ref commandQueue;
    id<MTLCommandBuffer> commandBuffer;

protected:
    id<MTLBuffer> createStagingBuffer(agpu_memory_heap_type heapType, size_t allocationSize);
};

template<agpu_memory_heap_type HT, size_t IC>
class AMtlImplicitResourceStagingCommandList : public AMtlImplicitResourceSetupCommandList
{
public:
    static constexpr agpu_memory_heap_type MemoryHeapType = HT;
    static constexpr size_t InitialCapacity = IC;

    AMtlImplicitResourceStagingCommandList(AMtlDevice &cdevice)
        : AMtlImplicitResourceSetupCommandList(cdevice),
        currentStagingBufferSize(0),
        currentStagingBufferPointer(nullptr),
        stagingBufferCapacity(0),
        stagingBufferBasePointer(nullptr),
        bufferHandle(nil)
    {
    }

    ~AMtlImplicitResourceStagingCommandList()
    {
    }

	void destroy()
	{
		bufferHandle = nil;
		AMtlImplicitResourceSetupCommandList::destroy();
	}

    void ensureValidCPUStagingBuffer(size_t requiredSize, size_t requiredAlignment)
    {
        if(stagingBufferCapacity < requiredSize)
        {
            stagingBufferCapacity = nextPowerOfTwo(requiredSize);
            stagingBufferCapacity = std::max(stagingBufferCapacity, size_t(InitialCapacity));
            if(bufferHandle)
            {
                bufferHandle = nil;
                stagingBufferBasePointer = nullptr;
            }

            bufferHandle = createStagingBuffer(MemoryHeapType, stagingBufferCapacity);
            stagingBufferBasePointer = reinterpret_cast<uint8_t*> (bufferHandle.contents);
        }

        currentStagingBufferSize = requiredSize;
        currentStagingBufferPointer = stagingBufferBasePointer;
    }

    bool uploadBufferData(id<MTLBuffer> destBuffer, size_t offset, size_t size)
    {
        @autoreleasepool {
            id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
            [blitEncoder copyFromBuffer: bufferHandle sourceOffset: 0
                toBuffer: destBuffer destinationOffset: offset
                size: size];
            [blitEncoder endEncoding];
            
            return true;
        }
    }

    bool readbackBufferData(id<MTLBuffer> sourceBuffer, size_t offset, size_t size)
    {
        @autoreleasepool {
            id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
            [blitEncoder copyFromBuffer: sourceBuffer sourceOffset: offset
                toBuffer: bufferHandle destinationOffset: 0
                size: size];
            [blitEncoder endEncoding];
            
            return true;
        }
    }

    bool uploadBufferDataToImage(id<MTLTexture> destImage, size_t level, size_t layerIndex, MTLRegion region, agpu_int pitch, agpu_int slicePitch)
    {
        @autoreleasepool {
            id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
            [blitEncoder copyFromBuffer: bufferHandle
                        sourceOffset: 0
                    sourceBytesPerRow: pitch
                sourceBytesPerImage: slicePitch
                            sourceSize: region.size
                            toTexture: destImage
                    destinationSlice: layerIndex
                    destinationLevel: level
                    destinationOrigin: region.origin];
            [blitEncoder endEncoding];
            return true;
        }
    }

    bool readbackImageDataToBuffer(id<MTLTexture> sourceImage, size_t level, size_t layerIndex, MTLRegion region, agpu_int pitch, agpu_int slicePitch)
    {
        @autoreleasepool {
            id<MTLBlitCommandEncoder> blitEncoder = [commandBuffer blitCommandEncoder];
            [blitEncoder copyFromTexture:sourceImage
                            sourceSlice: layerIndex
                            sourceLevel: level
                        sourceOrigin: region.origin
                            sourceSize: region.size
                            toBuffer: bufferHandle
                    destinationOffset:0
                destinationBytesPerRow:pitch
            destinationBytesPerImage:slicePitch];
            [blitEncoder endEncoding];
            return true;
        }
    }
    
    size_t currentStagingBufferSize;
    void *currentStagingBufferPointer;

protected:

    size_t stagingBufferCapacity;
    uint8_t *stagingBufferBasePointer;

    id<MTLBuffer> bufferHandle;
};

typedef AMtlImplicitResourceStagingCommandList<AGPU_MEMORY_HEAP_TYPE_HOST_TO_DEVICE, 2048*2048*4> AMtlImplicitResourceUploadCommandList;
typedef AMtlImplicitResourceStagingCommandList<AGPU_MEMORY_HEAP_TYPE_DEVICE_TO_HOST, 2048*2048*4> AMtlImplicitResourceReadbackCommandList;

} // End of namespace AgpuMetal

#endif //AGPU_METAL_IMPLICIT_RESOURCE_COMMAND_LIST_HPP
