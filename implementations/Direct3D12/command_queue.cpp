#include "command_queue.hpp"
#include "command_list.hpp"
#include "fence.hpp"

namespace AgpuD3D12
{

ADXCommandQueue::ADXCommandQueue(const agpu::device_ref &cdevice)
    : device(cdevice), finishFenceEvent(NULL)
{

}

ADXCommandQueue::~ADXCommandQueue()
{
    CloseHandle(finishFenceEvent);
}

agpu::command_queue_ref ADXCommandQueue::createDefault(const agpu::device_ref &device)
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ComPtr<ID3D12CommandQueue> d3dQueue;
    if (FAILED(deviceForDX->d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&d3dQueue))))
        return agpu::command_queue_ref();

    auto queue = agpu::makeObject<ADXCommandQueue> (device);
    auto queueObject = queue.as<ADXCommandQueue> ();
    queueObject->queue = d3dQueue;
    if (!queueObject->createFinishFence())
        return agpu::command_queue_ref();

    return queue;
}

bool ADXCommandQueue::createFinishFence()
{
    // Create transfer synchronization fence.
    if (FAILED(deviceForDX->d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&finishFence))))
        return false;
    finishFenceValue = 1;

    // Create an event handle to use for frame synchronization.
    finishFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
    if (finishFenceEvent == nullptr)
        return false;
    return true;
}

agpu_error ADXCommandQueue::addCommandList(const agpu::command_list_ref &command_list)
{
    CHECK_POINTER(command_list);

    ID3D12CommandList *lists[] = { command_list.as<ADXCommandList>()->commandList.Get() };

    queue->ExecuteCommandLists(1, lists);
    return AGPU_OK;
}

agpu_error ADXCommandQueue::addCommandListsAndSignalFence(agpu_uint count, agpu::command_list_ref* command_lists, const agpu::fence_ref& fence)
{
    std::vector<ID3D12CommandList*> lists(count);
    for (agpu_uint i = 0; i < count; ++i)
    {
        CHECK_POINTER(command_lists[i]);
        lists[i] = command_lists[i].as<ADXCommandList>()->commandList.Get();
    }

    queue->ExecuteCommandLists(count, lists.data());
    if (fence)
        signalFence(fence);
    return AGPU_OK;
}

agpu_error ADXCommandQueue::finishExecution()
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

agpu_error ADXCommandQueue::signalFence(const agpu::fence_ref &fence)
{
    CHECK_POINTER(fence);
    auto adxFence = fence.as<ADXFence> ();

    std::unique_lock<std::mutex> l(adxFence->fenceMutex);
    queue->Signal(adxFence->fence.Get(), adxFence->fenceValue++);

    return AGPU_OK;
}

agpu_error ADXCommandQueue::waitFence(const agpu::fence_ref &fence)
{
    CHECK_POINTER(fence);
    auto adxFence = fence.as<ADXFence> ();

    std::unique_lock<std::mutex> l(adxFence->fenceMutex);
    queue->Wait(adxFence->fence.Get(), adxFence->fenceValue - 1);
    return AGPU_OK;
}

} // End of namespace AgpuD3D12
