#ifndef AGPU_COMMAND_QUEUE_HPP
#define AGPU_COMMAND_QUEUE_HPP

#include "device.hpp"

struct _agpu_command_queue : public Object<_agpu_command_queue>
{
public:
    _agpu_command_queue(agpu_device *device);
    void lostReferences();

    static _agpu_command_queue *create(agpu_device *device, id<MTLCommandQueue> handle);

    agpu_error addCommandList ( agpu_command_list* command_list );
    agpu_error finishExecution (  );
    agpu_error signalFence ( agpu_fence* fence );
    agpu_error waitFence ( agpu_fence* fence );
    
    agpu_device *device;
    id<MTLCommandQueue> handle;
};

#endif //AGPU_COMMAND_QUEUE_HPP
