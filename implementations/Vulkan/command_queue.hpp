#ifndef AGPU_COMMAND_QUEUE_HPP
#define AGPU_COMMAND_QUEUE_HPP

#include "device.hpp"

namespace AgpuVulkan
{

struct AVkCommandQueue : public agpu::command_queue
{
public:
    AVkCommandQueue(const agpu::device_ref &device);
    ~AVkCommandQueue();

    static agpu::command_queue_ref create(const agpu::device_ref &device, agpu_uint queueFamilyIndex, agpu_uint queueIndex, VkQueue queue, agpu_command_queue_type type);

    virtual agpu_error addCommandList(const agpu::command_list_ref &command_list) override;
    virtual agpu_error finishExecution() override;
    virtual agpu_error signalFence(const agpu::fence_ref &fence) override;
    virtual agpu_error waitFence(const agpu::fence_ref &fence) override;

    bool supportsPresentingSurface(VkSurfaceKHR surface);

    agpu::device_weakref weakDevice;
    agpu_uint queueFamilyIndex;
    agpu_uint queueIndex;
    VkQueue queue;
    agpu_command_queue_type type;
};

} // End of namespace AgpuVulkan

#endif //AGPU_COMMAND_QUEUE_HPP
