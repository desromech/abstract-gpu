#ifndef AGPU_D3D12_PIPELINE_STATE_HPP
#define AGPU_D3D12_PIPELINE_STATE_HPP

#include "device.hpp"

struct _agpu_pipeline_state : public Object<_agpu_pipeline_state>
{
public:
    _agpu_pipeline_state();

    void lostReferences();

public:

};

#endif //AGPU_D3D12_PIPELINE_STATE_HPP
