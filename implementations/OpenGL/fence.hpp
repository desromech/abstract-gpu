#ifndef AGPU_GL_FENCE_HPP
#define AGPU_GL_FENCE_HPP

#include "device.hpp"

namespace AgpuGL
{

struct GLFence : public agpu::fence
{
public:
    GLFence();
    ~GLFence();

    static agpu::fence_ref create(const agpu::device_ref &device);

    agpu_error waitOnClient();

public:
    agpu::device_ref device;
    GLsync fenceObject;

    std::mutex mutex;
    std::condition_variable sendCommandCondition;
};

} // End of namespace AgpuGL

#endif //AGPU_GL_FENCE_HPP
