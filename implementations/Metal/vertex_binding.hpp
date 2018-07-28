#ifndef AGPU_METAL_VERTEX_BINDING_HPP
#define AGPU_METAL_VERTEX_BINDING_HPP

#include "device.hpp"
#include <vector>

struct _agpu_vertex_binding : public Object<_agpu_vertex_binding>
{
public:
    _agpu_vertex_binding(agpu_device *device);
    void lostReferences();

    static _agpu_vertex_binding *create(agpu_device *device, agpu_vertex_layout *layout);

    agpu_error bindBuffers ( agpu_uint count, agpu_buffer** vertex_buffers );
    agpu_error bindBuffersWithOffsets ( agpu_uint count, agpu_buffer** vertex_buffers, agpu_size *offsets );

    agpu_device *device;
    std::vector<agpu_buffer*> buffers;
    std::vector<agpu_size> offsets;
};

#endif //AGPU_METAL_VERTEX_BINDING_HPP
