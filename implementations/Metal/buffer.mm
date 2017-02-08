#include "buffer.hpp"

_agpu_buffer::_agpu_buffer(agpu_device *device)
    : device(device)
{
    handle = nil;
}

void _agpu_buffer::lostReferences()
{
    if(handle)
        [handle release];
}

agpu_buffer* _agpu_buffer::create ( agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data )
{
    if(!description)
        return nullptr;

    id<MTLBuffer> handle = nil;
    MTLResourceOptions options = MTLResourceOptionCPUCacheModeDefault;
    if(initial_data)
        handle = [device->device newBufferWithBytes: initial_data length: description->size options: options];
    else
        handle = [device->device newBufferWithLength: description->size options: options];

    auto result = new agpu_buffer(device);
    result->description = *description;
    result->handle = handle;
    return result;
}

agpu_pointer _agpu_buffer::map ( agpu_mapping_access flags )
{
    return handle.contents;
}

agpu_error _agpu_buffer::unmap (  )
{
    return AGPU_OK;
}

agpu_error _agpu_buffer::getDescription ( agpu_buffer_description* description )
{
    *description = this->description;
    return AGPU_OK;
}

agpu_error _agpu_buffer::uploadData ( agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(data)
    if(offset + size > handle.length)
        return AGPU_OUT_OF_BOUNDS;

    auto contents = reinterpret_cast<uint8_t*> (handle.contents);
    if(!contents)
        return AGPU_INVALID_OPERATION;

    memcpy(contents + offset, data, size);
    return AGPU_OK;
}

agpu_error _agpu_buffer::readData ( agpu_size offset, agpu_size size, agpu_pointer buffer )
{
    CHECK_POINTER(buffer)
    if(offset + size > handle.length)
        return AGPU_OUT_OF_BOUNDS;

    auto contents = reinterpret_cast<uint8_t*> (handle.contents);
    if(!contents)
        return AGPU_INVALID_OPERATION;

    memcpy(buffer, contents + offset, size);
    return AGPU_OK;
}

agpu_error _agpu_buffer::flushWhole (  )
{
    NSRange range;
    range.location = 0;
    range.length = description.size;
    [handle didModifyRange: range];
    return AGPU_OK;
}

agpu_error _agpu_buffer::invalidateWhole (  )
{
    NSRange range;
    range.location = 0;
    range.length = description.size;
    [handle didModifyRange: range];
    return AGPU_OK;
}

// The exported C interface.
AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->retain();
}

AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->release();
}

AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags )
{
    if(!buffer)
        return nullptr;
    return buffer->map(flags);
}

AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->unmap();
}

AGPU_EXPORT agpu_error agpuGetBufferDescription ( agpu_buffer* buffer, agpu_buffer_description* description )
{
    CHECK_POINTER(buffer);
    return buffer->getDescription(description);
}

AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(buffer);
    return buffer->uploadData(offset, size, data);
}

AGPU_EXPORT agpu_error agpuReadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(buffer);
    return buffer->readData(offset, size, data);
}

AGPU_EXPORT agpu_error agpuFlushWholeBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->flushWhole();
}

AGPU_EXPORT agpu_error agpuInvalidateWholeBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return buffer->invalidateWhole();
}
