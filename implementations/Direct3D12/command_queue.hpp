#ifndef AGPU_D3D12_COMMAND_QUEUE_HPP_
#define AGPU_D3D12_COMMAND_QUEUE_HPP_

#include "device.hpp"

struct _agpu_command_queue : public Object<_agpu_command_queue>
{
public:
    _agpu_command_queue();

    void lostReferences();

    static _agpu_command_queue *createDefault(agpu_device *device);

    agpu_error addCommandList(agpu_command_list* command_list);

public:

    agpu_device *device;
    ComPtr<ID3D12CommandQueue> queue;
};

#endif //AGPU_D3D12_COMMAND_QUEUE_HPP_