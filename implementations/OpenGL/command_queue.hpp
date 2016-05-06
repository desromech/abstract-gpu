#ifndef AGPU_COMMAND_QUEUE_HPP_
#define AGPU_COMMAND_QUEUE_HPP_

#include <functional>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "device.hpp"

class GpuCommand;

struct _agpu_command_queue: public Object<_agpu_command_queue>
{
public:
    _agpu_command_queue();

    static agpu_command_queue *create(agpu_device *device);

    agpu_error addCustomCommand(const std::function<void()> &command);

    void lostReferences();
    agpu_error addCommandList ( agpu_command_list* command_list );
    agpu_error finish();
    agpu_error signalFence ( agpu_fence* fence );
    agpu_error waitFence ( agpu_fence* fence );


public:
    void addCommand(GpuCommand *command);

    agpu_device *device;
};


#endif //AGPU_COMMAND_QUEUE_HPP_
