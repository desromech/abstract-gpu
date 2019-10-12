#ifndef AGPU_D3D12_SHADER_SIGNATURE_HPP
#define AGPU_D3D12_SHADER_SIGNATURE_HPP

#include <vector>
#include "device.hpp"
#include "shader_signature_builder.hpp"

namespace AgpuD3D12
{

class DescriptorTableMemoryAllocator
{
public:
	DescriptorTableMemoryAllocator();
    ~DescriptorTableMemoryAllocator();

	void setup(uint32_t maxDescriptorCount);

	int32_t allocate();
    void free(int32_t descriptorTableIndex);

private:
	uint32_t descriptorCount;
	int32_t nextFreeItem;
    std::vector<int32_t> freeList;
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

	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> descriptorTableHeapGPUBaseAddress;
	std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> descriptorTableHeapCPUBaseAddress;
	std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> nullDescriptorTables;
private:
	bool constructDescriptorTableAllocators();
	bool initializeDescriptorTable(const ShaderSignatureBindingBank &bank, const D3D12_CPU_DESCRIPTOR_HANDLE &tableHandle);

    std::mutex allocationMutex;
	std::vector<DescriptorTableMemoryAllocator> descriptorAllocators;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
