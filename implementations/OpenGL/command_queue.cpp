#include "command_queue.hpp"
#include "command_list.hpp"
#include "fence.hpp"

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
    GpuSignalFenceCommand(agpu_fence *fence)
        : fence(fence), signaled(false)
    {

    }

    virtual void execute()
    {
        std::unique_lock<std::mutex> l(fence->mutex);

        auto device = OpenGLContext::getCurrent()->device;
        if(fence->fenceObject)
            device->glDeleteSync(fence->fenceObject);

        fence->fenceObject = device->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        glFlush();
    }

    virtual void destroy()
    {
        std::unique_lock<std::mutex> l(fence->mutex);
        signaled = true;
        fence->sendCommandCondition.notify_all();
    }

    void wait()
    {
        {
            std::unique_lock<std::mutex> l(fence->mutex);
            while(!signaled)
                fence->sendCommandCondition.wait(l);
        }
    }

    agpu_fence *fence;
    bool signaled;
};

class GpuExecuteCommandList: public GpuCommand
{
public:
    GpuExecuteCommandList(agpu_command_list *command_list)
        : command_list(command_list)
    {
        command_list->retain();
    }

    ~GpuExecuteCommandList()
    {
        command_list->release();
    }

    virtual void execute()
    {
        command_list->execute();
    }

    virtual void destroy()
    {
        delete this;
    }

    agpu_command_list *command_list;
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

_agpu_command_queue::_agpu_command_queue()
    : isRunning_(false)
{
    context = nullptr;
}

agpu_command_queue *_agpu_command_queue::create(agpu_device *device)
{
	auto queue = new _agpu_command_queue();
    queue->device = device;
    queue->start();
	return queue;
}

void _agpu_command_queue::lostReferences()
{
    shutdown();
}

agpu_error _agpu_command_queue::addCommandList ( agpu_command_list* command_list )
{
	CHECK_POINTER(command_list);
    addCommand(new GpuExecuteCommandList(command_list));
	return AGPU_OK;
}

agpu_error _agpu_command_queue::addCustomCommand(const std::function<void()> &command)
{
    addCommand(new GpuCustomCommand(command));
    return AGPU_OK;
}

void _agpu_command_queue::addCommand(GpuCommand *command)
{
    std::unique_lock<std::mutex> l(controlMutex);
    queuedCommands.push_back(command);
    wakeCondition.notify_all();
}

agpu_error _agpu_command_queue::finish()
{
    GpuFinishCommand finishCommand;
    addCommand(&finishCommand);
    finishCommand.wait();
    return AGPU_OK;
}

void _agpu_command_queue::start()
{
    assert(!isRunning_);
    isShuttingDown_ = false;
    isRunning_ = true;
    std::thread t([this]() {
        queueThreadEntry();
    });
    queueThread.swap(t);
}

void _agpu_command_queue::shutdown()
{
    assert(isRunning_);
    {
        std::unique_lock<std::mutex> l(controlMutex);
        isShuttingDown_ = true;
        wakeCondition.notify_all();
    }
    queueThread.join();
}

agpu_error _agpu_command_queue::signalFence ( agpu_fence* fence )
{
    GpuSignalFenceCommand signalCommand(fence);
    addCommand(&signalCommand);
    signalCommand.wait();
    return AGPU_OK;
}

agpu_error _agpu_command_queue::waitFence ( agpu_fence* fence )
{
    GLsync fenceObject = nullptr;
    {
        std::unique_lock<std::mutex> l(fence->mutex);
        fenceObject = fence->fenceObject;
    }
    if(fenceObject)
        addCustomCommand([=]() {
            glFlush();
            device->glWaitSync(fenceObject, 0, -1);
        });
    return AGPU_OK;
}

void _agpu_command_queue::queueThreadEntry()
{
    context = device->createSecondaryContext(true);

    for(;;)
    {
        GpuCommand *nextCommand = nullptr;

        {
            std::unique_lock<std::mutex> l(controlMutex);
            while(!isShuttingDown_ && queuedCommands.empty())
                context->waitCondition(wakeCondition, l);

            if(isShuttingDown_)
                break;

            nextCommand = queuedCommands.back();
            queuedCommands.pop_back();
        }

        assert(nextCommand);
        nextCommand->execute();
        nextCommand->destroy();
    }

    // Destroy the context.
    context->destroy();
    delete context;
    context = nullptr;

}

AGPU_EXPORT agpu_error agpuAddCommandQueueReference ( agpu_command_queue* command_queue )
{
	CHECK_POINTER(command_queue);
	return command_queue->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue ( agpu_command_queue* command_queue )
{
	CHECK_POINTER(command_queue);
	return command_queue->release();
}

AGPU_EXPORT agpu_error agpuAddCommandList ( agpu_command_queue* command_queue, agpu_command_list* command_list )
{
	CHECK_POINTER(command_queue);
	return command_queue->addCommandList(command_list);
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
	return command_queue->finish();
}

AGPU_EXPORT agpu_error agpuSignalFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
    CHECK_POINTER(command_queue);
    return command_queue->signalFence(fence);
}

AGPU_EXPORT agpu_error agpuWaitFence ( agpu_command_queue* command_queue, agpu_fence* fence )
{
    CHECK_POINTER(command_queue);
    return command_queue->waitFence(fence);
}
