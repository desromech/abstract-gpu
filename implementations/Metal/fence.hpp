#ifndef AGPU_FENCE_HPP
#define AGPU_FENCE_HPP

#include "device.hpp"
#include <mutex>

namespace AgpuMetal
{
    
class AMtlFence : public agpu::fence
{
public:
    AMtlFence(const agpu::device_ref &device);
    ~AMtlFence();

    static agpu::fence_ref create(const agpu::device_ref &device);

    virtual agpu_error waitOnClient() override;
    agpu_error signalOnQueue(id<MTLCommandQueue> queue);

    agpu::device_weakref weakDevice;
    id<MTLCommandBuffer> fenceCommand;
    std::mutex mutex;
};

} // End of namespace AgpuMetal

#endif //AGPU_FENCE_HPP
