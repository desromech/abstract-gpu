#ifndef AGPU_FENCE_HPP
#define AGPU_FENCE_HPP

#include "device.hpp"
#include <mutex>

struct _agpu_fence : public Object<_agpu_fence>
{
public:
    _agpu_fence(agpu_device *device);
    void lostReferences();

    static agpu_fence *create(agpu_device *device);

    agpu_error waitOnClient();
    agpu_error signalOnQueue(id<MTLCommandQueue> queue);

    agpu_device *device;
    id<MTLCommandBuffer> fenceCommand;
    std::mutex mutex;
};

#endif //AGPU_FENCE_HPP
