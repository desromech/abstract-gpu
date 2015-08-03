#ifndef AGPU_D3D12_PIPELINE_BUILDER_HPP
#define AGPU_D3D12_PIPELINE_BUILDER_HPP

#include "device.hpp"

struct _agpu_pipeline_builder : public Object<_agpu_pipeline_builder>
{
public:
    _agpu_pipeline_builder();

    void lostReferences();

public:

};

#endif //AGPU_D3D12_PIPELINE_BUILDER_HPP