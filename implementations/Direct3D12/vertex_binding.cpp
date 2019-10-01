#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "buffer.hpp"

namespace AgpuD3D12
{

ADXVertexBinding::ADXVertexBinding(const agpu::device_ref &cdevice, const agpu::vertex_layout_ref &clayout)
    : device(cdevice), layout(clayout)
{
    auto count = layout.as<ADXVertexLayout> ()->vertexBufferCount;
    vertexBufferViews.resize(count);
    vertexBuffers.resize(count);
}

ADXVertexBinding::~ADXVertexBinding()
{
}

agpu::vertex_binding_ref ADXVertexBinding::create(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout)
{
    if (!layout)
        return agpu::vertex_binding_ref();

    return agpu::makeObject<ADXVertexBinding> (device, layout);
}

agpu_error ADXVertexBinding::bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers)
{
    return bindVertexBuffersWithOffsets(count, vertex_buffers, nullptr);
}

agpu_error ADXVertexBinding::bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size* offsets)
{
    CHECK_POINTER(vertex_buffers);
    if (count != vertexBuffers.size())
        return AGPU_ERROR;

    auto &strides = layout.as<ADXVertexLayout> ()->strides;

    for (agpu_uint i = 0; i < count; ++i)
    {
        auto buffer = vertex_buffers[i];
        CHECK_POINTER(buffer);

        // Store a reference to the vertex buffer
        vertexBuffers[i] = buffer;

        // Store the view.
        auto error = buffer.as<ADXBuffer> ()->createVertexBufferView(&vertexBufferViews[i], offsets ? offsets[i] : 0, strides[i]);
        if(error) return error;
    }

    return AGPU_OK;
}

} // End of namespace AgpuD3D12
