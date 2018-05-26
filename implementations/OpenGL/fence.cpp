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
    device->onMainContextBlocking([&]() {
        if(fenceObject)
        {
            GLenum waitReturn = GL_UNSIGNALED;
            while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED)
                waitReturn = device->glClientWaitSync(fenceObject, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
        	device->glDeleteSync(fenceObject);
        	fenceObject = nullptr;
        }
    });
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
