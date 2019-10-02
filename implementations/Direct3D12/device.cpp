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
#include "renderpass.hpp"
#include "swap_chain.hpp"
#include "texture.hpp"
#include "fence.hpp"
#include "platform.hpp"
#include "sampler.hpp"
#include "../Common/offline_shader_compiler.hpp"
#include "../Common/state_tracker_cache.hpp"

namespace AgpuD3D12
{

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

ADXDevice::ADXDevice()
	: renderTargetViewDescriptorSize(0),
	isOpened(false),
	isDebugEnabled(false)
{
}

ADXDevice::~ADXDevice()
{
    if (isOpened)
    {
        CloseHandle(transferFenceEvent);
    }
}

bool ADXDevice::checkDirect3D12Implementation(Direct3D12Platform *platform)
{
    // TODO: Implement this properly.
    platform->gpuCount = 1;
    return true;
}

agpu::device_ref ADXDevice::open(agpu_device_open_info* openInfo)
{
    auto device = agpu::makeObject<ADXDevice> ();
    if (!device.as<ADXDevice> ()->initialize(openInfo))
        return agpu::device_ref();

    return device;
}

bool ADXDevice::initialize(agpu_device_open_info* openInfo)
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
    defaultCommandQueue = ADXCommandQueue::createDefault(refFromThis<agpu::device> ());
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

agpu_error ADXDevice::withTransferQueue(std::function<agpu_error (const ComPtr<ID3D12CommandQueue> &)> function)
{
    std::unique_lock<std::mutex> l(transferMutex);
    return function(transferCommandQueue);
}

agpu_error ADXDevice::withTransferQueueAndCommandList(std::function<agpu_error(const ComPtr<ID3D12CommandQueue> &, const ComPtr<ID3D12GraphicsCommandList> &list)> function)
{
    std::unique_lock<std::mutex> l(transferMutex);
    transferCommandAllocator->Reset();
    transferCommandList->Reset(transferCommandAllocator.Get(), nullptr);
    return function(transferCommandQueue, transferCommandList);
}

agpu_error ADXDevice::waitForMemoryTransfer()
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

agpu::command_queue_ptr ADXDevice::getDefaultCommandQueue()
{
    return defaultCommandQueue.disownedNewRef();
}

agpu::swap_chain_ptr ADXDevice::createSwapChain(const agpu::command_queue_ref & commandQueue, agpu_swap_chain_create_info* swapChainInfo)
{
    return ADXSwapChain::create(refFromThis<agpu::device> (), commandQueue, swapChainInfo).disown();
}

agpu::buffer_ptr ADXDevice::createBuffer(agpu_buffer_description* description, agpu_pointer initial_data)
{
    return ADXBuffer::create(refFromThis<agpu::device> (), description, initial_data).disown();
}

agpu::vertex_layout_ptr ADXDevice::createVertexLayout()
{
    return ADXVertexLayout::create(refFromThis<agpu::device> ()).disown();
}

agpu::vertex_binding_ptr ADXDevice::createVertexBinding(const agpu::vertex_layout_ref & layout)
{
    return ADXVertexBinding::create(refFromThis<agpu::device> (), layout).disown();
}

agpu::shader_ptr ADXDevice::createShader(agpu_shader_type type)
{
    return ADXShader::create(refFromThis<agpu::device> (), type).disown();
}

agpu::shader_signature_builder_ptr ADXDevice::createShaderSignatureBuilder()
{
    return ADXShaderSignatureBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::pipeline_builder_ptr ADXDevice::createPipelineBuilder()
{
    return ADXPipelineBuilder::create(refFromThis<agpu::device> ()).disown();
}

agpu::compute_pipeline_builder_ptr ADXDevice::createComputePipelineBuilder()
{
    // TODO: Implement this.
    return nullptr;
}

agpu::command_allocator_ptr ADXDevice::createCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & queue)
{
    return ADXCommandAllocator::create(refFromThis<agpu::device> (), type, queue).disown();
}

agpu::command_list_ptr ADXDevice::createCommandList(agpu_command_list_type type, const agpu::command_allocator_ref & allocator, const agpu::pipeline_state_ref & initial_pipeline_state)
{
    return ADXCommandList::create(refFromThis<agpu::device> (), type, allocator, initial_pipeline_state).disown();
}

agpu_shader_language ADXDevice::getPreferredShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_BINARY;
}

agpu_shader_language ADXDevice::getPreferredIntermediateShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_SPIR_V;
}

agpu_shader_language ADXDevice::getPreferredHighLevelShaderLanguage()
{
    return AGPU_SHADER_LANGUAGE_HLSL;
}

agpu::framebuffer_ptr ADXDevice::createFrameBuffer(agpu_uint width, agpu_uint height, agpu_uint colorCount, agpu::texture_view_ref* colorViews, const agpu::texture_view_ref & depthStencilView)
{
    return ADXFramebuffer::create(refFromThis<agpu::device> (), width, height, colorCount, colorViews, depthStencilView).disown();
}

agpu::renderpass_ptr ADXDevice::createRenderPass(agpu_renderpass_description* description)
{
    return ADXRenderPass::create(refFromThis<agpu::device> (), description).disown();
}

agpu::texture_ptr ADXDevice::createTexture(agpu_texture_description* description)
{
    return ADXTexture::create(refFromThis<agpu::device> (), description).disown();
}

agpu::sampler_ptr ADXDevice::createSampler(agpu_sampler_description* description)
{
    return ADXSampler::create(refFromThis<agpu::device> (), description).disown();
}

agpu::fence_ptr ADXDevice::createFence()
{
    return ADXFence::create(refFromThis<agpu::device> ()).disown();
}

agpu_int ADXDevice::getMultiSampleQualityLevels(agpu_uint sample_count)
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels;
    memset(&levels, 0, sizeof(levels));
    levels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    levels.SampleCount = sample_count;
    if (FAILED(d3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &levels, sizeof(levels))))
        return 0;

    return levels.NumQualityLevels;
}

agpu_bool ADXDevice::hasTopLeftNdcOrigin()
{
    return false;
}

agpu_bool ADXDevice::hasBottomLeftTextureCoordinates()
{
    return false;
}

agpu_bool ADXDevice::isFeatureSupported(agpu_feature feature)
{
    switch (feature)
	{
	case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING: return true;
	case AGPU_FEATURE_COMMAND_LIST_REUSE: return true;
	case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE: return true;
    //case AGPU_FEATURE_VRDISPLAY: return isVRDisplaySupported;
    //case AGPU_FEATURE_VRINPUT_DEVICES: return isVRInputDevicesSupported;
	default: return false;
	}
}

agpu::vr_system_ptr ADXDevice::getVRSystem()
{
    // TODO: Implement this with OpenVR, Windows Mixed Reality, and the HoloLens streaming API.
    return nullptr;
}

agpu::offline_shader_compiler_ptr ADXDevice::createOfflineShaderCompiler()
{
	return AgpuCommon::GLSLangOfflineShaderCompiler::createForDevice(refFromThis<agpu::device> ()).disown();
}

agpu::state_tracker_cache_ptr ADXDevice::createStateTrackerCache(const agpu::command_queue_ref & command_queue_family)
{
	return AgpuCommon::StateTrackerCache::create(refFromThis<agpu::device> (), 0).disown();
}

} // End of namespace AgpuD3D12
