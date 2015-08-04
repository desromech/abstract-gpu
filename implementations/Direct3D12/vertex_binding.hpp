#ifndef AGPU_D3D12_VERTEX_BINDING_HPP
#define AGPU_D3D12_VERTEX_BINDING_HPP

#include <vector>
#include "device.hpp"

struct _agpu_vertex_binding : public Object<_agpu_vertex_binding>
{
public:
    _agpu_vertex_binding();

    void lostReferences();

    static agpu_vertex_binding* create(agpu_device* device, agpu_vertex_layout *layout);

    agpu_error bindVertexBuffers(agpu_uint count, agpu_buffer** vertex_buffers);

public:
    agpu_device *device;
    agpu_vertex_layout *layout;

    std::vector<D3D12_VERTEX_BUFFER_VIEW> vertexBufferViews;
    std::vector<agpu_buffer*> vertexBuffers;

};

#endif //AGPU_D3D12_VERTEX_BINDING_HPP
