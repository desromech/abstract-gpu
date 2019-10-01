#ifndef AGPU_D3D12_SHADER_SIGNATURE_HPP
#define AGPU_D3D12_SHADER_SIGNATURE_HPP

#include <vector>
#include "device.hpp"
#include "shader_signature_builder.hpp"

namespace AgpuD3D12
{

class DescriptorAllocator
{
public:
    DescriptorAllocator(int startIndex, int descriptorCount);
    ~DescriptorAllocator();

    int getStartIndex();
    int allocate();
    void free(int descriptorIndex);

private:
    int startIndex;
    int descriptorCount;
    int freeCount;
    std::vector<int> freeList;
};

enum class ShaderResourceViewType
{
    Buffer = 0,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture2DMS,
    Texture2DMSArray,
    Texture3D,
    TextureCube,
    TextureCubeArray,
    Count
};

enum class UnorderedAccessViewType
{
    Buffer = 0,
    Texture1D,
    Texture1DArray,
    Texture2D,
    Texture2DArray,
    Texture3D,
    Count
};

class ADXShaderSignature : public agpu::shader_signature
{
public:
    ADXShaderSignature(const agpu::device_ref &cdevice);
    ~ADXShaderSignature();

    static agpu::shader_signature_ref create(const agpu::device_ref &device, const ComPtr<ID3D12RootSignature> &rootSignature, ADXShaderSignatureBuilder *builder);

    virtual agpu::shader_resource_binding_ptr createShaderResourceBinding(agpu_uint element) override;

public:
    agpu::device_ref device;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12DescriptorHeap> shaderResourceViewHeap;
    ComPtr<ID3D12DescriptorHeap> samplerHeap;

	std::vector<ShaderSignatureBindingBank> banks;
	size_t pushConstantCount;

    UINT shaderResourceViewDescriptorSize;
    UINT samplerDescriptorSize;

private:
    agpu_uint maxBindingsCount[AGPU_SHADER_BINDING_TYPE_COUNT];
    DescriptorAllocator *descriptorAllocators[16];

    UINT shaderResourceViewDescriptorReservedSize;
    UINT samplerDescriptorReservedSize;

    UINT nullCbvDescriptorOffset;
    UINT nullUavDescriptorOffset[(int)UnorderedAccessViewType::Count];
    UINT nullSrvDescriptorOffset[(int)ShaderResourceViewType::Count];
    UINT nullSamplerDescriptorOffset;

    std::mutex allocationMutex;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
