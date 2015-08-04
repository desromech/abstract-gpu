#include "device.hpp"
#include "command_queue.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "pipeline_builder.hpp"
#include "shader.hpp"
#include "vertex_binding.hpp"
#include "vertex_layout.hpp"

inline void extractBindingRange(unsigned int bindingSet, int &rangeStart, int &rangeSize)
{
    int firstBit = 0;
    bool gotBit = false;
    int bitCount = 0;
    while (bindingSet != 0)
    {
        if (gotBit)
        {
            ++rangeSize;
        }
        else if(bindingSet & 1)
        {
            gotBit = true;
            rangeStart = bitCount;
            ++rangeSize;
        }

        ++bitCount;
        bindingSet >>= 1;
    }
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
        waitForPreviousFrame();
        CloseHandle(frameFenceEvent);
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
    window = (HWND)openInfo->window;
    frameCount = openInfo->doublebuffer ? 2 : 1;
    isDebugEnabled = openInfo->debugLayer;

    // Get the window size.
    getWindowSize();

    // Enable the debug layer
    if (openInfo->debugLayer)
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
        }
    }

    ComPtr<IDXGIFactory4> factory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&factory))))
        return false;

    // Create the device
    if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice))))
        return false;

    // Create the default command queue
    defaultCommandQueue = agpu_command_queue::createDefault(this);
    if (!defaultCommandQueue)
        return false;

    // Create the swap chain
    DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    swapChainDesc.BufferCount = frameCount;
    swapChainDesc.BufferDesc.Width = windowWidth;
    swapChainDesc.BufferDesc.Height = windowHeight;
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // TODO: Pick a proper format
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.OutputWindow = window;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.Windowed = TRUE;

    ComPtr<IDXGISwapChain> oldSwapChain;
    if (FAILED(factory->CreateSwapChain(defaultCommandQueue->queue.Get(), &swapChainDesc, &oldSwapChain)))
        return false;

    if (FAILED(oldSwapChain.As(&swapChain)))
        return false;

    frameIndex = swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
        renderTargetViewHeapDesc.NumDescriptors = frameCount;
        renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        if (FAILED(d3dDevice->CreateDescriptorHeap(&renderTargetViewHeapDesc, IID_PPV_ARGS(&renderTargetViewHeap))))
            return false;

        renderTargetViewDescriptorSize = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // Create frame resources.
    {
        D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle(renderTargetViewHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV for each frame.
        for (int i = 0; i < frameCount; i++)
        {
            if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&mainFrameBufferTargets[i]))))
                return false;

            d3dDevice->CreateRenderTargetView(mainFrameBufferTargets[i].Get(), nullptr, renderTargetViewHandle);
            renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
        }
    }

    // Create frame synchronization fence.
    {
        if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&frameFence))))
            return false;
        frameFenceValue = 1;

        // Create an event handle to use for frame synchronization.
        frameFenceEvent = CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);
        if (frameFenceEvent == nullptr)
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

bool _agpu_device::getWindowSize()
{
    RECT rect;
    GetClientRect(window, &rect);
    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    // Check for changes.
    bool res = windowWidth != width || windowHeight != height;

    // Update the width and the height.
    windowWidth = width;
    windowHeight = height;
    return res;
}

agpu_error _agpu_device::swapBuffers()
{
    ERROR_IF_FAILED(swapChain->Present(1, 0));
    return waitForPreviousFrame();
}

agpu_error _agpu_device::waitForPreviousFrame()
{
    // Signal the fence.
    auto fence = frameFenceValue;
    ERROR_IF_FAILED(defaultCommandQueue->queue->Signal(frameFence.Get(), fence));
    ++frameFenceValue;

    // Wait until previous frame is finished.
    if (frameFence->GetCompletedValue() < fence)
    {
        ERROR_IF_FAILED(frameFence->SetEventOnCompletion(fence, frameFenceEvent));
        WaitForSingleObject(frameFenceEvent, INFINITE);
    }

    frameIndex = swapChain->GetCurrentBackBufferIndex();

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
    return device->defaultCommandQueue;
}

AGPU_EXPORT agpu_error agpuSwapBuffers(agpu_device* device)
{
    CHECK_POINTER(device);
    return device->swapBuffers();
}

AGPU_EXPORT agpu_buffer* agpuCreateBuffer(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data)
{
    return nullptr;
}

AGPU_EXPORT agpu_vertex_binding* agpuCreateVertexBinding(agpu_device* device, agpu_vertex_layout *layout)
{
    return nullptr;
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
    return agpu_command_list::create(device, allocator, initial_pipeline_state);
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_BINARY;
}

AGPU_EXPORT agpu_shader_language agpuGetPreferredHighLevelShaderLanguage(agpu_device* device)
{
    return AGPU_SHADER_LANGUAGE_HLSL;
}
