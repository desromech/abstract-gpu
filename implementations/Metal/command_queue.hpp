#ifndef AGPU_COMMAND_QUEUE_HPP
#define AGPU_COMMAND_QUEUE_HPP

#include "device.hpp"
#include <mutex>

namespace AgpuMetal
{

class AMtlCommandQueue : public agpu::command_queue
{
public:
    AMtlCommandQueue(const agpu::device_ref &device);
    ~AMtlCommandQueue();

    static agpu::command_queue_ref create(const agpu::device_ref &device, id<MTLCommandQueue> handle);

    virtual agpu_error addCommandList(const agpu::command_list_ref &command_list) override;
    virtual agpu_error addCommandListsAndSignalFence(agpu_uint count, agpu::command_list_ref* command_list, const agpu::fence_ref & fence) override;
    virtual agpu_error finishExecution() override;
    virtual agpu_error signalFence(const agpu::fence_ref &fence) override;
    virtual agpu_error waitFence(const agpu::fence_ref &fence) override;

    agpu::device_weakref weakDevice;
    id<MTLCommandQueue> handle;

private:
    agpu::fence_ref finishFence;
    std::mutex finishFenceMutex;
};

} // End of namespace AgpuMetal

#endif //AGPU_COMMAND_QUEUE_HPP
