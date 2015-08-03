#ifndef AGPU_D3D12_VERTEX_BINDING_HPP
#define AGPU_D3D12_VERTEX_BINDING_HPP

#include "device.hpp"

struct _agpu_vertex_binding : public Object<_agpu_vertex_binding>
{
public:
    _agpu_vertex_binding();

    void lostReferences();

public:

};

#endif //AGPU_D3D12_VERTEX_BINDING_HPP
