#include "command_queue.hpp"
#include "command_list.hpp"

_agpu_command_queue::_agpu_command_queue()
{

}

void _agpu_command_queue::lostReferences()
{

}

_agpu_command_queue *_agpu_command_queue::createDefault(agpu_device *device)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ComPtr<ID3D12CommandQueue> d3dQueue;
    if (FAILED(device->d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3dQueue))))
        return nullptr;

    std::unique_ptr<agpu_command_queue> queue(new agpu_command_queue());
    queue->device = device;
    queue->queue = d3dQueue;
    if (!queue->createFinishFence())
        return nullptr;

    return queue.release();
}

bool _agpu_command_queue::createFinishFence()
{
    // Create transfer synchronization fence.
    if (FAILED(device->d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&finishFence))))
        return false;
    finishFenceValue = 1;

    // Create an event handle to use for frame synchronization.
    finishFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (finishFenceEvent == nullptr)
        return false;
    return true;
}

agpu_error _agpu_command_queue::addCommandList(agpu_command_list* command_list)
{
    ID3D12CommandList *lists[] = { command_list->commandList.Get() };

    queue->ExecuteCommandLists(1, lists);
    return AGPU_OK;
}

agpu_error _agpu_command_queue::finish()
{
    std::unique_lock<std::mutex> l(finishLock);

    // Signal the fence.
    auto fence = finishFenceValue;
    ERROR_IF_FAILED(queue->Signal(finishFence.Get(), fence));
    ++finishFenceValue;

    // Wait until previous frame is finished.
    if (finishFence->GetCompletedValue() < fence)
    {
        ERROR_IF_FAILED(finishFence->SetEventOnCompletion(fence, finishFenceEvent));
        WaitForSingleObject(finishFenceEvent, INFINITE);
    }

    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddCommandQueueReference(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
    return command_queue->retain();
}

AGPU_EXPORT agpu_error agpuReleaseCommandQueue(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
    return command_queue->release();
}

AGPU_EXPORT agpu_error agpuAddCommandList(agpu_command_queue* command_queue, agpu_command_list* command_list)
{
    CHECK_POINTER(command_queue);
    return command_queue->addCommandList(command_list);
}

AGPU_EXPORT agpu_error agpuFinishQueueExecution(agpu_command_queue* command_queue)
{
    CHECK_POINTER(command_queue);
    return command_queue->finish();
}
