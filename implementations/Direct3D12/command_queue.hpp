#ifndef AGPU_D3D12_COMMAND_QUEUE_HPP_
#define AGPU_D3D12_COMMAND_QUEUE_HPP_

#include "device.hpp"

struct _agpu_command_queue : public Object<_agpu_command_queue>
{
public:
    _agpu_command_queue();

    void lostReferences();

    agpu_error addCommandList(agpu_command_list* command_list);

public:
    
};

#endif //AGPU_D3D12_COMMAND_QUEUE_HPP_