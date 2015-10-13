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
    agpu_error finish();

public:

    agpu_device *device;
    ComPtr<ID3D12CommandQueue> queue;

private:
    bool createFinishFence();

    std::mutex finishLock;
    HANDLE finishFenceEvent;
    ComPtr<ID3D12Fence> finishFence;
    UINT64 finishFenceValue;
};

#endif //AGPU_D3D12_COMMAND_QUEUE_HPP_