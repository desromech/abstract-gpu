#ifndef AGPU_D3D12_SHADER_HPP
#define AGPU_D3D12_SHADER_HPP

#include "device.hpp"

struct _agpu_shader: public Object<_agpu_shader>
{
public:
    _agpu_shader();

    void lostReferences();

public:

};

#endif //AGPU_D3D12_SHADER_HPP
