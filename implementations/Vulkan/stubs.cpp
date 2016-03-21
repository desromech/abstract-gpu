#include <AGPU/agpu.h>

/* Methods for interface agpu_buffer. */

AGPU_EXPORT agpu_error agpuAddBufferReference ( agpu_buffer* buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseBuffer ( agpu_buffer* buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_pointer agpuMapBuffer ( agpu_buffer* buffer, agpu_mapping_access flags )
{
    return nullptr;
}

AGPU_EXPORT agpu_error agpuUnmapBuffer ( agpu_buffer* buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuGetBufferDescription ( agpu_buffer* buffer, agpu_buffer_description* description )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuUploadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReadBufferData ( agpu_buffer* buffer, agpu_size offset, agpu_size size, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_vertex_binding. */

AGPU_EXPORT agpu_error agpuAddVertexBindingReference ( agpu_vertex_binding* vertex_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseVertexBinding ( agpu_vertex_binding* vertex_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindVertexBuffers ( agpu_vertex_binding* vertex_binding, agpu_uint count, agpu_buffer** vertex_buffers )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_shader_resource_binding. */

AGPU_EXPORT agpu_error agpuAddShaderResourceBindingReference ( agpu_shader_resource_binding* shader_resource_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseShaderResourceBinding ( agpu_shader_resource_binding* shader_resource_binding )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindUniformBuffer ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindUniformBufferRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindTexture ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuBindTextureArrayRange ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuCreateSampler ( agpu_shader_resource_binding* shader_resource_binding, agpu_int location, agpu_sampler_description* description )
{
    return AGPU_UNIMPLEMENTED;
}


/* Methods for interface agpu_fence. */

AGPU_EXPORT agpu_error agpuAddFenceReference ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}

AGPU_EXPORT agpu_error agpuWaitOnClient ( agpu_fence* fence )
{
    return AGPU_UNIMPLEMENTED;
}
