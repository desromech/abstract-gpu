#include "platform.hpp"
#include "device.hpp"
#include "../Common/offline_shader_compiler.hpp"
#include <mutex>
#include <locale>
#include <codecvt>

namespace AgpuD3D12
{
static agpu::platform_ref theDirect3D12Platform;

agpu_bool Direct3D12AdapterDesc::isFeatureSupported(agpu_feature feature)
{
    switch (feature)
    {
    case AGPU_FEATURE_PERSISTENT_MEMORY_MAPPING: return true;
    case AGPU_FEATURE_COHERENT_MEMORY_MAPPING: return true;
    case AGPU_FEATURE_PERSISTENT_COHERENT_MEMORY_MAPPING: return true;
    case AGPU_FEATURE_COMMAND_LIST_REUSE: return true;
    case AGPU_FEATURE_NON_EMULATED_COMMAND_LIST_REUSE: return true;

    case AGPU_FEATURE_DUAL_SOURCE_BLENDING: return true;
    case AGPU_FEATURE_GEOMETRY_SHADER: return true;
    case AGPU_FEATURE_TESSELLATION_SHADER: return true;
    case AGPU_FEATURE_COMPUTE_SHADER: return true;
    case AGPU_FEATURE_MULTI_DRAW_INDIRECT: return false;
    case AGPU_FEATURE_DRAW_INDIRECT: return false;
    case AGPU_FEATURE_TEXTURE_COMPRESSION_BC: return true;
    case AGPU_FEATURE_TEXTURE_COMPRESSION_ETC2: return false;
    case AGPU_FEATURE_TEXTURE_COMPRESSION_ASTC_LDR: return false;
    case AGPU_FEATURE_SHADER_CLIP_DISTANCE: return true;
    case AGPU_FEATURE_SHADER_CULL_DISTANCE: return true;
    case AGPU_FEATURE_SHADER_FLOAT_64: return false;
    case AGPU_FEATURE_SHADER_INT_64: return false;
    case AGPU_FEATURE_SHADER_INT_16: return true;
    case AGPU_FEATURE_SAMPLE_SHADING: return true;
    case AGPU_FEATURE_FILL_MODE_NON_SOLID: return true;
    default: return false;
    }
}

agpu_uint Direct3D12AdapterDesc::getLimitValue(agpu_limit limit)
{
    switch (limit)
    {
    case AGPU_LIMIT_NON_COHERENT_ATOM_SIZE: return 256;
    case AGPU_LIMIT_MIN_MEMORY_MAP_ALIGNMENT: return 256;
    case AGPU_LIMIT_MIN_TEXEL_BUFFER_OFFSET_ALIGNMENT: return 256;
    case AGPU_LIMIT_MIN_UNIFORM_BUFFER_OFFSET_ALIGNMENT: return 256;
    case AGPU_LIMIT_MIN_STORAGE_BUFFER_OFFSET_ALIGNMENT: return 256;
    case AGPU_LIMIT_DEDICATED_VIDEO_MEMORY_IN_MB: return agpu_uint(desc.DedicatedVideoMemory >> 20);
    case AGPU_LIMIT_AVAILABLE_VIDEO_MEMORY_IN_MB: return agpu_uint((desc.DedicatedVideoMemory + desc.SharedSystemMemory) >> 20);
    default: return 0;
    }
}

bool Direct3D12AdapterDesc::fetchFromAdapterAndDevice(const ComPtr<IDXGIAdapter1>& adapter, const ComPtr<ID3D12Device>& device)
{
    if (FAILED(adapter->GetDesc1(&desc)))
        return false;

    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ARCHITECTURE1, &architecture, sizeof(architecture))))
        return false;

    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t> convert;
    name = convert.to_bytes(std::wstring(desc.Description));

    if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
    {
        deviceType = AGPU_DEVICE_TYPE_CPU;
    }
    else
    {
        if (architecture.UMA)
            deviceType = AGPU_DEVICE_TYPE_INTEGRATED_GPU;
        else
            deviceType = AGPU_DEVICE_TYPE_DISCRETE_GPU;
    }
    return true;
}

Direct3D12Platform::Direct3D12Platform()
    : isSupported(0)
{
    isSupported = checkDirect3D12Implementation();
}

bool Direct3D12Platform::checkDirect3D12Implementation()
{
    // Create the factory.
    ComPtr<IDXGIFactory1> dxgiFactory;
    if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))))
        return false;

    // Enumerate the adapters.
    {
        ComPtr<IDXGIAdapter1> adapter;
        UINT i = 0;
        while (dxgiFactory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND)
        {
            // Create a device for testing the support for d3d12.
            ComPtr<ID3D12Device> d3dDevice;
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&d3dDevice))))
            {
                Direct3D12AdapterDesc desc = {};
                if (desc.fetchFromAdapterAndDevice(adapter, d3dDevice))
                {
                    adapterDescs.push_back(desc);
                }
            }
            ++i;
        }
    }

    return !adapterDescs.empty();
}

Direct3D12Platform::~Direct3D12Platform()
{
}

agpu::device_ptr Direct3D12Platform::openDevice(agpu_device_open_info* openInfo)
{
    return ADXDevice::open(openInfo).disown();
}

agpu_cstring Direct3D12Platform::getName()
{
    return "Direct3D12";
}

agpu_size Direct3D12Platform::getGpuCount()
{
    return agpu_size(adapterDescs.size());
}

agpu_cstring Direct3D12Platform::getGpuName(agpu_size gpu_index)
{
    if (gpu_index < adapterDescs.size())
        return adapterDescs[gpu_index].name.c_str();
    return "";
}

agpu_device_type Direct3D12Platform::getGpuDeviceType(agpu_size gpu_index)
{
    if (gpu_index < adapterDescs.size())
        return adapterDescs[gpu_index].deviceType;
    return AGPU_DEVICE_TYPE_OTHER;
}

agpu_bool Direct3D12Platform::isFeatureSupportedOnGPU(agpu_size gpu_index, agpu_feature feature)
{
    if (gpu_index < adapterDescs.size())
        return adapterDescs[gpu_index].isFeatureSupported(feature);
    return false;
}

agpu_uint Direct3D12Platform::getLimitValueOnGPU(agpu_size gpu_index, agpu_limit limit)
{
    if (gpu_index < adapterDescs.size())
        return adapterDescs[gpu_index].getLimitValue(limit);
    return 0;
}

agpu_int Direct3D12Platform::getVersion()
{
    return 1200;
}

agpu_int Direct3D12Platform::getImplementationVersion()
{
    return 1200;
}

agpu_bool Direct3D12Platform::hasRealMultithreading()
{
    return true;
}

agpu_bool Direct3D12Platform::isNative()
{
    return true;
}

agpu_bool Direct3D12Platform::isCrossPlatform()
{
    return false;
}

agpu::offline_shader_compiler_ptr Direct3D12Platform::createOfflineShaderCompiler()
{
    return AgpuCommon::GLSLangOfflineShaderCompiler::create().disown();
}

} // End of namespace AgpuD3D12


AGPU_EXPORT agpu_error agpuGetPlatforms(agpu_size numplatforms, agpu_platform** platforms, agpu_size* ret_numplatforms)
{
    using namespace AgpuD3D12;
    static std::once_flag platformCreatedFlag;
    std::call_once(platformCreatedFlag, []{
        theDirect3D12Platform = agpu::makeObject<Direct3D12Platform> ();
    });

    if(!theDirect3D12Platform.as<Direct3D12Platform> ()->isSupported)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 0;
        return AGPU_OK;
    }

    if (!platforms && numplatforms == 0)
    {
        CHECK_POINTER(ret_numplatforms);
        *ret_numplatforms = 1;
        return AGPU_OK;
    }

    if(ret_numplatforms)
        *ret_numplatforms = 1;
    platforms[0] = reinterpret_cast<agpu_platform*> (theDirect3D12Platform.asPtrWithoutNewRef());
    return AGPU_OK;

}
