#ifndef AGPU_VULKAN_VERTEX_BINDING_HPP
#define AGPU_VULKAN_VERTEX_BINDING_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkVertexBinding : public agpu::vertex_binding
{
public:
    AVkVertexBinding(const agpu::device_ref &device);
    ~AVkVertexBinding();

    static agpu::vertex_binding_ref create(const agpu::device_ref &device, const agpu::vertex_layout_ref &layout);

    virtual agpu_error bindVertexBuffers(agpu_uint count, agpu::buffer_ref* vertex_buffers) override;
	virtual agpu_error bindVertexBuffersWithOffsets(agpu_uint count, agpu::buffer_ref* vertex_buffers, agpu_size* offsets) override;

    agpu::device_ref device;

    std::vector<agpu::buffer_ref> vertexBuffers;
    std::vector<VkBuffer> vulkanBuffers;
    std::vector<VkDeviceSize> offsets;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_VERTEX_BINDING_HPP
