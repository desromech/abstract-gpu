#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "buffer.hpp"

namespace AgpuMetal
{
    
AMtlVertexBinding::AMtlVertexBinding(const agpu::device_ref &device)
    : device(device)
{
}

AMtlVertexBinding::~AMtlVertexBinding()
{
}

agpu::vertex_binding_ref AMtlVertexBinding::create(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout)
{
    if(!layout)
        return agpu::vertex_binding_ref();
    auto amtlLayout = layout.as<AMtlVertexLayout> ();
    
    auto result = agpu::makeObject<AMtlVertexBinding> (device);
    auto bindings = result.as<AMtlVertexBinding> ();
    bindings->buffers.resize(amtlLayout->vertexStrides.size());
    bindings->offsets.resize(amtlLayout->vertexStrides.size());
    return result;
}

agpu_error AMtlVertexBinding::bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers)
{
    return bindVertexBuffersWithOffsets(count, vertex_buffers, nullptr);
}

agpu_error AMtlVertexBinding::bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size *offsets)
{
    CHECK_POINTER(vertex_buffers);
    if(count != buffers.size())
        return AGPU_INVALID_PARAMETER;

    for(size_t i = 0; i < count; ++i)
    {
        auto newBuffer = vertex_buffers[i];
        this->buffers[i] = newBuffer;
        this->offsets[i] = offsets ? offsets[i] : 0;
    }

    return AGPU_OK;
}

} // End of namespace AgpuMetal
