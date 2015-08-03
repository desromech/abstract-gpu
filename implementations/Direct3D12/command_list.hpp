#ifndef AGPU_D3D12_COMMAND_LIST_HPP_
#define AGPU_D3D12_COMMAND_LIST_HPP_

#include "device.hpp"

struct _agpu_command_list : public Object<_agpu_command_list>
{
public:
    _agpu_command_list();

    void lostReferences();


};

#endif //AGPU_D3D12_COMMAND_LIST_HPP_