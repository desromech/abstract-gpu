#include "buffer.hpp"

namespace AgpuMetal
{
    
AMtlBuffer::AMtlBuffer(const agpu::device_ref &device)
    : device(device)
{
    handle = nil;
}

AMtlBuffer::~AMtlBuffer()
{
    if(handle)
        [handle release];
}

agpu::buffer_ref AMtlBuffer::create ( const agpu::device_ref &device, agpu_buffer_description* description, agpu_pointer initial_data )
{
    if(!description)
        return agpu::buffer_ref();

    id<MTLBuffer> handle = nil;
    MTLResourceOptions options = MTLResourceOptionCPUCacheModeDefault;
    if(initial_data)
        handle = [deviceForMetal->device newBufferWithBytes: initial_data length: description->size options: options];
    else
        handle = [deviceForMetal->device newBufferWithLength: description->size options: options];

    auto result = agpu::makeObject<AMtlBuffer> (device);
    auto buffer = result.as<AMtlBuffer> ();
    buffer->description = *description;
    buffer->handle = handle;
    return result;
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

    auto contents = reinterpret_cast<uint8_t*> (handle.contents);
    if(!contents)
        return AGPU_INVALID_OPERATION;

    memcpy(contents + offset, data, size);
    return AGPU_OK;
}

agpu_error AMtlBuffer::readBufferData(agpu_size offset, agpu_size size, agpu_pointer buffer)
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
    NSRange range;
    range.location = 0;
    range.length = description.size;
    [handle didModifyRange: range];
    return AGPU_OK;
}

} // End of namespace AgpuMetal
