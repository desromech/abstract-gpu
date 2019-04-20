#include "fence.hpp"

namespace AgpuGL
{

GLFence::GLFence()
{
    fenceObject = nullptr;
}

GLFence::~GLFence()
{
    if(fenceObject)
    {
        deviceForGL->onMainContextBlocking([&]() {
            deviceForGL->glDeleteSync(fenceObject);
        });
    }
}

agpu::fence_ref GLFence::create(const agpu::device_ref &device)
{
    auto result = agpu::makeObject<GLFence> ();
    result.as<GLFence> ()->device = device;
    return result;
}

agpu_error GLFence::waitOnClient()
{
    deviceForGL->onMainContextBlocking([&]() {
        if(fenceObject)
        {
            GLenum waitReturn = GL_UNSIGNALED;
            while (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED)
                waitReturn = deviceForGL->glClientWaitSync(fenceObject, GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
        	deviceForGL->glDeleteSync(fenceObject);
        	fenceObject = nullptr;
        }
    });
    return AGPU_OK;
}

} // End of namespace AgpuGL
