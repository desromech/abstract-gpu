#ifndef _AGPU_METAL_DEVICE_HPP_
#define _AGPU_METAL_DEVICE_HPP_

#include "object.hpp"
#import <Metal/Metal.h>

struct _agpu_device : public Object<_agpu_device>
{
public:
    void lostReferences();

    static agpu_device *open(agpu_device_open_info *openInfo);

    agpu_command_queue* getDefaultCommandQueue();

    id<MTLDevice> device;


private:
    _agpu_device();

    agpu_command_queue *mainCommandQueue;

};

#endif //_AGPU_METAL_DEVICE_HPP_
