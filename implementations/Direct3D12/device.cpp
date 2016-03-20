#include "device.hpp"
#include "command_queue.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "pipeline_builder.hpp"
#include "shader.hpp"
#include "shader_signature_builder.hpp"
#include "shader_resource_binding.hpp"
#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "fence.hpp"

void printError(const char *format, ...)
{
    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, 1024, format, args);
#ifdef _WIN32
    OutputDebugStringA(buffer);
#else
    fputs(buffer, stderr);
#endif
    va_end(args);
}

_agpu_device::_agpu_device()
{
    defaultCommandQueue = nullptr;
    isOpened = false;
}

void _agpu_device::lostReferences()
{

    if (isOpened)
    {
        CloseHandle(transferFenceEvent);
    }

    if (defaultCommandQueue)
        defaultCommandQueue->release();
}

agpu_device *_agpu_device::open(agpu_device_open_info* openInfo)
{
    auto device = new agpu_device();
    if (!device->initialize(openInfo))
    {
        device->release();
        return nullptr;
    }

    return device;
}

bool _agpu_device::initialize(agpu_device_open_info* openInfo)
{
    // Read some of the parameters.
    isDebugEnabled = openInfo->debug_layer;

    // Enable the debug layer
    if (openInfo->debug_layer)
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }

    // Create the device
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice))))
        return false;

    // Create the default command queue
    defaultCommandQueue = agpu_command_queue::createDefault(this);
    if (!defaultCommandQueue)
        return false;

    // Create the transfer command queue.
    {
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        if (FAILED(d3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&transferCommandQueue))))
            return false;

        // Create the transfer command allocator.
        if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&transferCommandAllocator))))
            return false;

        // Create the transfer command list.
        if (FAILED(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, transferCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&transferCommandList))))
            return false;

        if (FAILED(transferCommandList->Close()))
            return false;
    }

    // Create transfer synchronization fence.
    {
        if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&transferFence))))
            return false;
        transferFenceValue = 1;

        // Create an event handle to use for frame synchronization.
        transferFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
        if (transferFenceEvent == nullptr)
            return false;
    }

    isOpened = true;

    return true;
}

agpu_error _agpu_device::withTransferQueue(std::function<agpu_error (const ComPtr<ID3D12CommandQueue> &)> function)
{
    std::unique_lock<std::mutex> l(transferMutex);
    return function(transferCommandQueue);
}

agpu_error _agpu_device::withTransferQueueAndCommandList(std::function<agpu_error(const ComPtr<ID3D12CommandQueue> &, const ComPtr<ID3D12GraphicsCommandList> &list)> function)
{
    std::unique_lock<std::mutex> l(transferMutex);
    transferCommandAllocator->Reset();
    transferCommandList->Reset(transferCommandAllocator.Get(), nullptr);
    return function(transferCommandQueue, transferCommandList);
}

agpu_error _agpu_device::waitForMemoryTransfer()
{
    // Signal the fence.
    auto fence = transferFenceValue;
    ERROR_IF_FAILED(transferCommandQueue->Signal(transferFence.Get(), fence));
    ++transferFenceValue;

    // Wait until previous frame is finished.
    if (transferFence->GetCompletedValue() < fence)
    {
        ERROR_IF_FAILED(transferFence->SetEventOnCompletion(fence, transferFenceEvent));
        WaitForSingleObject(transferFenceEvent, INFINITE);
    }

    return AGPU_OK;
}

agpu_int _agpu_device::getMultiSampleQualityLevels(agpu_uint sample_count)
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels;
    memset(&levels, 0, sizeof(levels));
    levels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    levels.SampleCount = sample_count;
    if (FAILED(d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels, sizeof(levels))))
        return 0;

    return levels.NumQualityLevels;
}

// Exported C functions
AGPU_EXPORT agpu_error agpuAddDeviceReference(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->retain();
}

AGPU_EXPORT agpu_error agpuReleaseDevice(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->release();
}

AGPU_EXPORT agpu_command_queue* agpuGetDefaultCommandQueue(agpu_device* device)
{
    if (!device)
        return nullptr;
    device->defaultCommandQueue->retain();
    return device->defaultCommandQueue;
}

AGPU_EXPORT agpu_swap_chain* agpuCreateSwapChain(agpu_device* device, agpu_command_queue* commandQueue, agpu_swap_chain_create_info* swapChainInfo)
{
    if (!device)
        return nullptr;
    return agpu_swap_chain::create(device, commandQueue, swapChainInfo);
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    if (!device)
        return nullptr;
    return agpu_buffer::create(device, description, initial_data);
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* device, agpu_vertex_layout *layout)
{
    if (!device)
        return nullptr;

    return agpu_vertex_binding::create(device, layout);
}

AGPU_EXPORT agpu_vertex_layout* agpuCreateVertexLayout(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_vertex_layout::create(device);
}

AGPU_EXPORT agpu_shader* agpuCreateShader(agpu_device* device, agpu_shader_type type)
{
    if (!device)
        return nullptr;
    return agpu_shader::create(device, type);
}

AGPU_EXPORT agpu_shader_signature_builder* agpuCreateShaderSignatureBuilder(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_shader_signature_builder::create(device);
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder(agpu_device* device)
{
    if (!device)
        return nullptr;

    return agpu_pipeline_builder::create(device);
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator(agpu_device* device, agpu_command_list_type type)
{
    if (!device)
        return nullptr;
    return agpu_command_allocator::create(device, type);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList(agpu_device* device, agpu_command_list_type type, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    if (!device)
        return nullptr;
    return agpu_command_list::create(device, type, allocator, initial_pipeline_state);
}


AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_BINARY;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_HLSL;
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu_texture_view_description* colorViews, agpu_texture_view_description* depthStencilView)
{
    if (!device)
        return nullptr;
    return agpu_framebuffer::create(device, width, height, colorCount, colorViews, depthStencilView);
}

AGPU_EXPORT agpu_texture* agpuCreateTexture(agpu_device* device, agpu_texture_description* description)
{
    if (!device)
        return nullptr;

    return agpu_texture::create(device, description);
}

AGPU_EXPORT agpu_fence* agpuCreateFence(agpu_device* device)
{
    if (!device)
        return nullptr;

    return agpu_fence::create(device);
}

AGPU_EXPORT agpu_int agpuGetMultiSampleQualityLevels(agpu_device* device, agpu_uint sample_count)
{
    if (!device)
        return 0;
    return device->getMultiSampleQualityLevels(sample_count);
}
