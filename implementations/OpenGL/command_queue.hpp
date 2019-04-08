#ifndef AGPU_COMMAND_QUEUE_HPP_
#define AGPU_COMMAND_QUEUE_HPP_

#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "device.hpp"

namespace AgpuGL
{

class GpuCommand;

struct GLCommandQueue: public agpu::command_queue
{
public:
    GLCommandQueue();
    ~GLCommandQueue();

    static agpu::command_queue_ref create(const agpu::device_ref &device);

    agpu_error addCustomCommand(const std::function<void()> &command);

    virtual agpu_error addCommandList (const agpu::command_list_ref &command_list) override;
    virtual agpu_error finishExecution() override;
    virtual agpu_error signalFence(const agpu::fence_ref &fence) override;
    virtual agpu_error waitFence(const agpu::fence_ref &fence) override;


public:
    void addCommand(GpuCommand *command);

    agpu::device_weakref weakDevice;
};

} // End of namespace AgpuGL

#endif //AGPU_COMMAND_QUEUE_HPP_
