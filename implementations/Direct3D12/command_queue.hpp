#ifndef AGPU_D3D12_COMMAND_QUEUE_HPP_
#define AGPU_D3D12_COMMAND_QUEUE_HPP_

#include "device.hpp"

namespace AgpuD3D12
{

class ADXCommandQueue : public agpu::command_queue
{
public:
    ADXCommandQueue(const agpu::device_ref &cdevice);
    ~ADXCommandQueue();

    static agpu::command_queue_ref createDefault(const agpu::device_ref &device);

    virtual agpu_error addCommandList(const agpu::command_list_ref &command_list) override;
    virtual agpu_error finishExecution() override;
    virtual agpu_error signalFence(const agpu::fence_ref &fence) override;
    virtual agpu_error waitFence(const agpu::fence_ref &fence) override;

public:

    agpu::device_ref device;
    ComPtr<ID3D12CommandQueue> queue;

private:
    bool createFinishFence();

    std::mutex finishLock;
    HANDLE finishFenceEvent;
    ComPtr<ID3D12Fence> finishFence;
    UINT64 finishFenceValue;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_COMMAND_QUEUE_HPP_
