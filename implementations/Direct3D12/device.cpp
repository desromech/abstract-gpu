#include "device.hpp"
#include "command_queue.hpp"
#include "command_allocator.hpp"
#include "command_list.hpp"
#include "pipeline_state.hpp"
#include "pipeline_builder.hpp"
#include "compute_pipeline_builder.hpp"
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
#include "../Common/window_scraper.hpp"

namespace AgpuD3D12
{


agpu_error convertErrorCode(HRESULT errorCode)
{
    if (!FAILED(errorCode))
        return AGPU_OK;

    return AGPU_ERROR;
}

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
	isDebugEnabled(false),
    implicitResourceSetupCommandList(*this),
    implicitResourceUploadCommandList(*this),
    implicitResourceReadbackCommandList(*this)
{
}

ADXDevice::~ADXDevice()
{
}

agpu::device_ref ADXDevice::open(Direct3D12Platform* platform, agpu_device_open_info* openInfo)
{
    auto device = agpu::makeObject<ADXDevice> ();
    if (!device.as<ADXDevice> ()->initialize(platform, openInfo))
        return agpu::device_ref();

    return device;
}

bool ADXDevice::initialize(Direct3D12Platform* platform, agpu_device_open_info* openInfo)
{
    // Get the dxgi factory to retrieve the adapter.
    ComPtr<IDXGIFactory1> dxgiFactory;
    if (FAILED(CreateDXGIFactory2(openInfo->debug_layer ? DXGI_CREATE_FACTORY_DEBUG : 0, IID_PPV_ARGS(&dxgiFactory))))
        return false;

    // Find the selected adapter.
    if (openInfo->gpu_index < 0)
        openInfo->gpu_index = 0;
    
    if (size_t(openInfo->gpu_index) >= platform->adapterDescs.size())
        return false;

    auto platformAdapterDesc = platform->adapterDescs[openInfo->gpu_index];

    // Enumerate the adapters.
    {
        ComPtr<IDXGIAdapter1> enumAdapter;
        UINT i = 0;
        while (dxgiFactory->EnumAdapters1(i, &enumAdapter) != DXGI_ERROR_NOT_FOUND)
        {
            DXGI_ADAPTER_DESC1 desc;
            if (SUCCEEDED(enumAdapter->GetDesc1(&desc)))
            {
                if (platformAdapterDesc.desc.AdapterLuid.LowPart == desc.AdapterLuid.LowPart &&
                    platformAdapterDesc.desc.AdapterLuid.HighPart == desc.AdapterLuid.HighPart)
                {
                    dxgiAdapter = enumAdapter;
                    adapterDesc.reset(new Direct3D12AdapterDesc());
                    break;
                }
            }
            ++i;
        }
    }


    // Make sure we actually found the adapter.
    if (!dxgiAdapter)
        return false;

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

    // Create the device.
    if (FAILED(D3D12CreateDevice(dxgiAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice))))
        return false;

    // Query the device specs.
    if (!adapterDesc->fetchFromAdapterAndDevice(dxgiAdapter, d3dDevice))
        return false;

    // Create the default command queue
    defaultCommandQueue = ADXCommandQueue::createDefault(refFromThis<agpu::device> ());
    if (!defaultCommandQueue)
        return false;

    // Create the memory allocator.
    {
        D3D12MA::ALLOCATOR_DESC allocatorDesc = {};
        allocatorDesc.pDevice = d3dDevice.Get();
        allocatorDesc.PreferredBlockSize = 64*1024*1024; // Reduce the default block size to 64 MB.
        if(FAILED(D3D12MA::CreateAllocator(&allocatorDesc, &memoryAllocator)))
            return false;
    }

    // Initialize the implicit resources
    auto transferQueue = defaultCommandQueue.as<ADXCommandQueue> ()->queue;
    if(!implicitResourceSetupCommandList.initializeWithQueue(transferQueue) ||
       !implicitResourceUploadCommandList.initializeWithQueue(transferQueue) ||
       !implicitResourceReadbackCommandList.initializeWithQueue(transferQueue))
       return false;

    isOpened = true;

    return true;
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
	return ADXComputePipelineBuilder::create(refFromThis<agpu::device>()).disown();
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

agpu_int ADXDevice::getMultiSampleQualityLevels(agpu_texture_format format, agpu_uint sample_count)
{
	if (sample_count == 1)
		return 1;

    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS levels = {};
    levels.Format = DXGI_FORMAT(format);
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

agpu_cstring ADXDevice::getName()
{
    return adapterDesc->name.c_str();
}

agpu_device_type ADXDevice::getType()
{
    return adapterDesc->deviceType;
}

agpu_bool ADXDevice::isFeatureSupported(agpu_feature feature)
{
    switch (feature)
	{
    //case AGPU_FEATURE_VRDISPLAY: return isVRDisplaySupported;
    //case AGPU_FEATURE_VRINPUT_DEVICES: return isVRInputDevicesSupported;
	default: return adapterDesc->isFeatureSupported(feature);
	}
}

agpu_uint ADXDevice::getLimitValue(agpu_limit limit)
{
    return adapterDesc->getLimitValue(limit);
}

agpu::vr_system_ptr ADXDevice::getVRSystem()
{
    // TODO: Implement this with OpenVR, Windows Mixed Reality, and the HoloLens streaming API.
    return nullptr;
}

agpu::window_scraper_ptr ADXDevice::createWindowScraper()
{
    return AgpuCommon::createWindowScraper(refFromThis<agpu::device> ()).disown();
}

agpu::offline_shader_compiler_ptr ADXDevice::createOfflineShaderCompiler()
{
	return AgpuCommon::GLSLangOfflineShaderCompiler::createForDevice(refFromThis<agpu::device> ()).disown();
}

agpu::state_tracker_cache_ptr ADXDevice::createStateTrackerCache(const agpu::command_queue_ref & command_queue_family)
{
	return AgpuCommon::StateTrackerCache::create(refFromThis<agpu::device> (), 0).disown();
}

agpu_error ADXDevice::finishExecution()
{
	// TODO: Finish the execution of all of the command queues.
	return defaultCommandQueue->finishExecution();
}

} // End of namespace AgpuD3D12
