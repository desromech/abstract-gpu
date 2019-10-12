#include "vertex_binding.hpp"
#include "buffer.hpp"

namespace AgpuVulkan
{

AVkVertexBinding::AVkVertexBinding(const agpu::device_ref &device)
    : device(device)
{
}

AVkVertexBinding::~AVkVertexBinding()
{
}

agpu::vertex_binding_ref AVkVertexBinding::create(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout)
{
    return agpu::makeObject<AVkVertexBinding> (device);
}

agpu_error AVkVertexBinding::bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers)
{
    return bindVertexBuffersWithOffsets(count, vertex_buffers, nullptr);
}


agpu_error AVkVertexBinding::bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size* offsets)
{
    for (size_t i = 0; i < count; ++i)
    {
        CHECK_POINTER(vertex_buffers[i]);
        if ((vertex_buffers[i].as<AVkBuffer> ()->description.usage_modes & AGPU_ARRAY_BUFFER) == 0)
            return AGPU_INVALID_PARAMETER;
    }

    this->vertexBuffers.resize(count);
    this->vulkanBuffers.resize(count);
    this->offsets.resize(count);

    for (size_t i = 0; i < count; ++i)
    {
        auto buffer = vertex_buffers[i];
        this->vertexBuffers[i] = buffer;
        this->vulkanBuffers[i] = buffer.as<AVkBuffer> ()->handle;
        this->offsets[i] = offsets ? offsets[i] : 0;
    }

    return AGPU_OK;
}

} // End of namespace AgpuVulkan
