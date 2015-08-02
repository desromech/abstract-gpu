#ifndef AGPU_COMMAND_QUEUE_HPP_
#define AGPU_COMMAND_QUEUE_HPP_

#include "device.hpp"

struct _agpu_command_queue: public Object<_agpu_command_queue>
{
public:
    _agpu_command_queue();
    
    static agpu_command_queue *create(agpu_device *device);

    void lostReferences();
    agpu_error addCommandList ( agpu_command_list* command_list );
    
public:
    agpu_device *device;
};


#endif //AGPU_COMMAND_QUEUE_HPP_