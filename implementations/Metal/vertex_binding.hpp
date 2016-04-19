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

    agpu_device *device;
    std::vector<agpu_buffer*> buffers;
};

#endif //AGPU_METAL_VERTEX_BINDING_HPP
