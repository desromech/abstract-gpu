#ifndef AGPU_GL_FENCE_HPP
#define AGPU_GL_FENCE_HPP

#include "device.hpp"

struct _agpu_fence : public Object<_agpu_fence>
{
public:
    _agpu_fence();

    void lostReferences();

    static agpu_fence* create(agpu_device* device);

    agpu_error waitOnClient();

public:
    agpu_device *device;
    GLsync fenceObject;

    std::mutex mutex;
    std::condition_variable sendCommandCondition;
};
#endif //AGPU_GL_FENCE_HPP
