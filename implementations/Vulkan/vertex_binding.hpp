#ifndef AGPU_VULKAN_VERTEX_BINDING_HPP
#define AGPU_VULKAN_VERTEX_BINDING_HPP

#include "device.hpp"

struct _agpu_vertex_binding : public Object<_agpu_vertex_binding>
{
    _agpu_vertex_binding(agpu_device *device);
    void lostReferences();

    static agpu_vertex_binding *create(agpu_device *device, agpu_vertex_layout* layout);

    agpu_error bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers);

    agpu_device *device;

    std::vector<agpu_buffer*> vertexBuffers;
    std::vector<VkBuffer> vulkanBuffers;
    std::vector<VkDeviceSize> offsets;
};

#endif //AGPU_VULKAN_VERTEX_BINDING_HPP