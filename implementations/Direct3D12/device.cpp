#include "device.hpp"
#include "command_queue.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "pipeline_builder.hpp"
#include "shader.hpp"
#include "shader_resource_binding.hpp"
#include "vertex_binding.hpp"
#include "vertex_layout.hpp"
#include "buffer.hpp"
#include "framebuffer.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"

_agpu_device::_agpu_device()
{
    defaultCommandQueue = nullptr;
    isOpened = false;

    for (int i = 0; i < 4; ++i)
    {
        shaderResourceBindingOffsets[i] = 0;
    }
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

    {
        // Describe and create a constant buffer view descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = 16;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        shaderResourceViewDescriptorSize= d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

        for (int i = 0; i < 4; ++i)
        {
            if (FAILED(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&shaderResourcesViewHeaps[i]))))
                return false;
        }
        
    }

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

    // Create the graphics root signature.
    if (createGraphicsRootSignature() < 0)
        return false;

    return true;
}

agpu_error _agpu_device::createGraphicsRootSignature()
{
    D3D12_ROOT_SIGNATURE_DESC desc;
    memset(&desc, 0, sizeof(desc));
    desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    D3D12_DESCRIPTOR_RANGE ranges[1];
    D3D12_ROOT_PARAMETER rootParameters[1];
    memset(ranges, 0, sizeof(ranges));
    memset(rootParameters, 0, sizeof(rootParameters));

    // Simple descriptor table
    // 16 Constant buffers
    ranges[0].BaseShaderRegister = 0;
    ranges[0].NumDescriptors = 16;
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    ranges[0].RegisterSpace = 0;
    ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;;

    // Shared uniform buffers.
    rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
    rootParameters[0].DescriptorTable.pDescriptorRanges = &ranges[0];
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    desc.NumParameters = 1;
    desc.pParameters = rootParameters;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
        return AGPU_ERROR;

    ComPtr<ID3D12RootSignature> rootSignature;
    if (FAILED(d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&graphicsRootSignature))))
        return AGPU_ERROR;

    return AGPU_OK;
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

AGPU_EXPORT agpu_shader_resource_binding* agpuCreateShaderResourceBinding(agpu_device* device, agpu_int bindingBank)
{
    if (!device)
        return nullptr;

    return agpu_shader_resource_binding::create(device, bindingBank);
}

AGPU_EXPORT agpu_pipeline_builder* agpuCreatePipelineBuilder(agpu_device* device)
{
    if (!device)
        return nullptr;

    return agpu_pipeline_builder::create(device);
}

AGPU_EXPORT agpu_command_allocator* agpuCreateCommandAllocator(agpu_device* device)
{
    if (!device)
        return nullptr;
    return agpu_command_allocator::create(device);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandList(agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    if (!device)
        return nullptr;
    return agpu_command_list::create(device, CommandListType::Direct, allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_command_list* agpuCreateCommandListBundle(agpu_device* device, agpu_command_allocator* allocator, agpu_pipeline_state* initial_pipeline_state)
{
    if (!device)
        return nullptr;
    return agpu_command_list::create(device, CommandListType::Bundle, allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_BINARY;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_HLSL;
}

AGPU_EXPORT agpu_framebuffer* agpuCreateFrameBuffer(agpu_device* device, agpu_uint width, agpu_uint height, agpu_uint renderTargetCount, agpu_bool hasDepth, agpu_bool hasStencil)
{
    if (!device)
        return nullptr;
    return agpu_framebuffer::create(device, width, height, renderTargetCount, hasDepth, hasStencil);
}

AGPU_EXPORT agpu_texture* agpuCreateTexture(agpu_device* device, agpu_texture_description* description, agpu_pointer initialData)
{
    if (!device)
        return nullptr;

    return agpu_texture::create(device, description, initialData);
}
