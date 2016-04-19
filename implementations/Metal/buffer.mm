#include "buffer.hpp"

_agpu_buffer::_agpu_buffer(agpu_device *device)
    : device(device)
{
    handle = nil;
}

void _agpu_buffer::lostReferences()
{
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

agpu_pointer _agpu_buffer::mapBuffer ( agpu_mapping_access flags )
{
    return nullptr;
}

agpu_error _agpu_buffer::unmapBuffer (  )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_buffer::getDescription ( agpu_buffer_description* description )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_buffer::uploadData ( agpu_size offset, agpu_size size, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_buffer::readData ( agpu_size offset, agpu_size size, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface.
AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags )
{
    if(!buffer)
        return nullptr;
    return nullptr;
}

AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer )
{
    CHECK_POINTER(buffer);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetBufferDescription ( agpu_buffer* buffer, agpu_buffer_description* description )
{
    CHECK_POINTER(buffer);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(buffer);
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    CHECK_POINTER(buffer);
    return AGPU_UNIMPLEMENTED;
}
