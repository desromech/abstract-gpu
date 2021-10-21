#include "implicit_resource_command_list.hpp"
#include "command_queue.hpp"
#include "constants.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{
AMtlImplicitResourceSetupCommandList::AMtlImplicitResourceSetupCommandList(AMtlDevice &cdevice)
    : device(cdevice)
{
    AgpuProfileConstructor(AMtlImplicitResourceSetupCommandList);
}

AMtlImplicitResourceSetupCommandList::~AMtlImplicitResourceSetupCommandList()
{
    AgpuProfileDestructor(AMtlImplicitResourceSetupCommandList);
}

void AMtlImplicitResourceSetupCommandList::destroy()
{
    if(!commandBuffer)
        return;

    commandBuffer = nil;
}

bool AMtlImplicitResourceSetupCommandList::setupCommandBuffer()
{
    id<MTLCommandQueue> queue = commandQueue.as<AMtlCommandQueue> ()->handle;
    @autoreleasepool {
        commandBuffer = [queue commandBuffer];
    }
    return commandBuffer != nil;
}

bool AMtlImplicitResourceSetupCommandList::submitCommandBuffer()
{
    [commandBuffer commit];
    [commandBuffer waitUntilCompleted];
    commandBuffer = nil;
    return true;
}

id<MTLBuffer> AMtlImplicitResourceSetupCommandList::createStagingBuffer(agpu_memory_heap_type heapType, size_t allocationSize)
{
    MTLResourceOptions options = mapBufferMemoryHeapType(heapType);
    return [device.device newBufferWithLength: allocationSize options: options];
}

}
