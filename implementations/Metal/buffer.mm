#include "buffer.hpp"
#include "constants.hpp"
#include "../Common/memory_profiler.hpp"

namespace AgpuMetal
{

AMtlBuffer::AMtlBuffer(const agpu::device_ref &device)
    : device(device)
{
    AgpuProfileConstructor(AMtlBuffer);
}

AMtlBuffer::~AMtlBuffer()
{
    AgpuProfileDestructor(AMtlBuffer);
}

agpu::buffer_ref AMtlBuffer::create ( const agpu::device_ref &device, agpu_buffer_description* description, agpu_pointer initial_data )
{
    if(!description)
        return agpu::buffer_ref();
    
    @autoreleasepool {
        id<MTLBuffer> handle = nil;
        MTLResourceOptions options = mapBufferMemoryHeapType(description->heap_type);
        if(initial_data && description->heap_type != AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL)
            handle = [deviceForMetal->device newBufferWithBytes: initial_data length: description->size options: options];
        else
            handle = [deviceForMetal->device newBufferWithLength: description->size options: options];

        auto result = agpu::makeObject<AMtlBuffer> (device);
        auto buffer = result.as<AMtlBuffer> ();
        buffer->description = *description;
        buffer->handle = handle;

        if(initial_data && description->heap_type == AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL)
            buffer->uploadBufferData(0, description->size, initial_data);
        
        return result;
    }
}

agpu_pointer AMtlBuffer::mapBuffer(agpu_mapping_access flags)
{
    return handle.contents;
}

agpu_error AMtlBuffer::unmapBuffer()
{
    return AGPU_OK;
}

agpu_error AMtlBuffer::getDescription(agpu_buffer_description* description)
{
    *description = this->description;
    return AGPU_OK;
}

agpu_error AMtlBuffer::uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data)
{
    CHECK_POINTER(data)
    if(offset + size > handle.length)
        return AGPU_OUT_OF_BOUNDS;
        
    if(description.heap_type != AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL)
    {
        auto contents = reinterpret_cast<uint8_t*> (handle.contents);
        if(!contents)
            return AGPU_INVALID_OPERATION;

        memcpy(contents + offset, data, size);
        return AGPU_OK;
    }
    
    bool uploadResult = false;
    deviceForMetal->withUploadCommandListDo(size, 1, [&](AMtlImplicitResourceUploadCommandList &uploadList) {
        // Do we need to stream the buffer upload?.
        if(size <= uploadList.currentStagingBufferSize)
        {
            memcpy(uploadList.currentStagingBufferPointer, data, size);
            uploadResult = uploadList.setupCommandBuffer()
                && uploadList.uploadBufferData(handle, offset, size)
                && uploadList.submitCommandBuffer();
        }
        else
        {
            abort();
        }
    });

    return uploadResult ? AGPU_OK : AGPU_ERROR;
}

agpu_error AMtlBuffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer buffer)
{
    CHECK_POINTER(buffer)
    if(offset + size > handle.length)
        return AGPU_OUT_OF_BOUNDS;

    if(description.heap_type != AGPU_MEMORY_HEAP_TYPE_DEVICE_LOCAL)
    {
        auto contents = reinterpret_cast<uint8_t*> (handle.contents);
        if(!contents)
            return AGPU_INVALID_OPERATION;

        memcpy(buffer, contents + offset, size);
    }
    
    bool readbackResult = false;
    deviceForMetal->withReadbackCommandListDo(size, 1, [&](AMtlImplicitResourceReadbackCommandList &readbackList) {
        // Do we need to stream the buffer upload?.
        if(size <= readbackList.currentStagingBufferSize)
        {
            readbackResult = readbackList.setupCommandBuffer()
                && readbackList.readbackBufferData(handle, offset, size)
                && readbackList.submitCommandBuffer();
            memcpy(buffer, readbackList.currentStagingBufferPointer, size);
        }
        else
        {
            abort();
        }
    });

    return readbackResult ? AGPU_OK : AGPU_ERROR;
}

agpu_error AMtlBuffer::flushWholeBuffer (  )
{
    NSRange range;
    range.location = 0;
    range.length = description.size;
    [handle didModifyRange: range];
    return AGPU_OK;
}

agpu_error AMtlBuffer::invalidateWholeBuffer (  )
{
    // FIXME: Implement this properly.
    return AGPU_OK;
}

} // End of namespace AgpuMetal
