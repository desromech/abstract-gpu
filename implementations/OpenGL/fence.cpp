#include "fence.hpp"

_agpu_fence::_agpu_fence()
{
    fenceObject = nullptr;
}

void _agpu_fence::lostReferences()
{
    if(fenceObject)
    {
        device->onMainContextBlocking([&]() {
            device->glDeleteSync(fenceObject);
        });
    }
}

agpu_fence* _agpu_fence::create(agpu_device* device)
{
    std::unique_ptr<agpu_fence> fence(new agpu_fence());
    fence->device = device;
    return fence.release();
}

agpu_error _agpu_fence::waitOnClient()
{
    if(fenceObject)
    {
        device->onMainContextBlocking([&]() {
            device->glClientWaitSync(fenceObject, 0, -1);
        });
    }
    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddFenceReference ( agpu_fence* fence )
{
    CHECK_POINTER(fence);
    return fence->retain();
}

AGPU_EXPORT agpu_error agpuReleaseFenceReference ( agpu_fence* fence )
{
    CHECK_POINTER(fence);
    return fence->release();
}

AGPU_EXPORT agpu_error agpuWaitOnClient ( agpu_fence* fence )
{
    CHECK_POINTER(fence);
    return fence->waitOnClient();
}
