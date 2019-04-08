#include "command_queue.hpp"
#include "command_list.hpp"
#include "fence.hpp"

namespace AgpuGL
{

class GpuCommand
{
public:
    virtual ~GpuCommand() {}

    virtual void execute() = 0;
    virtual void destroy() = 0;
};

class GpuFinishCommand: public GpuCommand
{
public:
    GpuFinishCommand()
        : finished(false)
    {

    }

    virtual void execute()
    {
        OpenGLContext::getCurrent()->finish();
    }

    virtual void destroy()
    {
        {
            std::unique_lock<std::mutex> l(mutex);
            finished = true;
            finishedCondition.notify_all();
        }
    }

    void wait()
    {
        {
            std::unique_lock<std::mutex> l(mutex);
            while(!finished)
                finishedCondition.wait(l);
        }
    }

    bool finished;
    std::mutex mutex;
    std::condition_variable finishedCondition;
};

class GpuSignalFenceCommand: public GpuCommand
{
public:
    GpuSignalFenceCommand(const agpu::fence_ref &fence)
        : fence(fence), signaled(false)
    {

    }

    virtual void execute()
    {
        auto glFence = fence.as<GLFence> ();
        std::unique_lock<std::mutex> l(glFence->mutex);

        auto device = OpenGLContext::getCurrent()->weakDevice.lock();
        if(glFence->fenceObject)
            deviceForGL->glDeleteSync(glFence->fenceObject);

        glFence->fenceObject = deviceForGL->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glFlush();
    }

    virtual void destroy()
    {
        auto glFence = fence.as<GLFence> ();
        std::unique_lock<std::mutex> l(glFence->mutex);
        signaled = true;
        glFence->sendCommandCondition.notify_all();
    }

    void wait()
    {
        auto glFence = fence.as<GLFence> ();
        {
            std::unique_lock<std::mutex> l(glFence->mutex);
            while(!signaled)
                glFence->sendCommandCondition.wait(l);
        }
    }

    agpu::fence_ref fence;
    bool signaled;
};

class GpuExecuteCommandList: public GpuCommand
{
public:
    GpuExecuteCommandList(const agpu::command_list_ref &command_list)
        : command_list(command_list)
    {
    }

    ~GpuExecuteCommandList()
    {
    }

    virtual void execute()
    {
        command_list.as<GLCommandList> ()->execute();
    }

    virtual void destroy()
    {
        delete this;
    }

    agpu::command_list_ref command_list;
};

class GpuCustomCommand : public GpuCommand
{
public:
    GpuCustomCommand(const std::function<void()> &command)
        : command(command)
    {
    }

    ~GpuCustomCommand()
    {
    }

    virtual void execute()
    {
        command();
    }

    virtual void destroy()
    {
        delete this;
    }

    std::function<void()> command;
};

GLCommandQueue::GLCommandQueue()
{
}

GLCommandQueue::~GLCommandQueue()
{
}

agpu::command_queue_ref GLCommandQueue::create(const agpu::device_ref &device)
{
	auto result = agpu::makeObject<GLCommandQueue> ();
    result.as<GLCommandQueue> ()->weakDevice = device;
	return result;
}

agpu_error GLCommandQueue::addCommandList(const agpu::command_list_ref &command_list)
{
	CHECK_POINTER(command_list);
    addCommand(new GpuExecuteCommandList(command_list));
	return AGPU_OK;
}

agpu_error GLCommandQueue::addCustomCommand(const std::function<void()> &command)
{
    addCommand(new GpuCustomCommand(command));
    return AGPU_OK;
}

void GLCommandQueue::addCommand(GpuCommand *command)
{
    lockWeakDeviceForGL->mainContextJobQueue.addJob(new AsyncJob([=] {
        command->execute();
        command->destroy();
    }, true));
}

agpu_error GLCommandQueue::finishExecution()
{
    GpuFinishCommand finishCommand;
    addCommand(&finishCommand);
    finishCommand.wait();
    return AGPU_OK;
}

agpu_error GLCommandQueue::signalFence(const agpu::fence_ref &fence )
{
    GpuSignalFenceCommand signalCommand(fence);
    addCommand(&signalCommand);
    signalCommand.wait();
    return AGPU_OK;
}

agpu_error GLCommandQueue::waitFence(const agpu::fence_ref &fence )
{
    GLsync fenceObject = nullptr;
    {
        auto glFence = fence.as<GLFence> ();
        std::unique_lock<std::mutex> l(glFence->mutex);
        fenceObject = glFence->fenceObject;
    }
    if(fenceObject)
        addCustomCommand([=]() {
            glFlush();
            lockWeakDeviceForGL->glWaitSync(fenceObject, 0, -1);
        });
    return AGPU_OK;
}

} // End of namespace AgpuGL
