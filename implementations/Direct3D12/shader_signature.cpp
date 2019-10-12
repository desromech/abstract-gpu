#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"
#include <assert.h>
#include <algorithm>

namespace AgpuD3D12
{

DescriptorTableMemoryAllocator::DescriptorTableMemoryAllocator()
	: descriptorCount(0), nextFreeItem(-1)
{
}

DescriptorTableMemoryAllocator::~DescriptorTableMemoryAllocator()
{
}

void DescriptorTableMemoryAllocator::setup(uint32_t maxDescriptorCount)
{

	freeList.resize(maxDescriptorCount, -1);
	for (int i = 0; i < maxDescriptorCount - 1; ++i) {
		freeList[i] = i + 1;
	}

	nextFreeItem = 0;
}

void DescriptorTableMemoryAllocator::free(int32_t descriptorTableIndex)
{
	assert(descriptorTableIndex >= 0);

    freeList[descriptorTableIndex] = nextFreeItem;
	nextFreeItem = descriptorTableIndex;
}

int32_t DescriptorTableMemoryAllocator::allocate()
{
    if (nextFreeItem < 0)
        return -1;
	
	auto result = nextFreeItem;
	nextFreeItem = freeList[nextFreeItem];
	return result;
}

ADXShaderSignature::ADXShaderSignature(const agpu::device_ref &cdevice)
    : device(cdevice), pushConstantCount(0)
{
}

ADXShaderSignature::~ADXShaderSignature()
{
}

agpu::shader_signature_ref ADXShaderSignature::create(const agpu::device_ref& device, const ComPtr<ID3D12RootSignature>& rootSignature, ADXShaderSignatureBuilder* builder)
{
	auto result = agpu::makeObject<ADXShaderSignature>(device);
	auto adxShaderSignature = result.as<ADXShaderSignature>();
	adxShaderSignature->rootSignature = rootSignature;
	adxShaderSignature->banks = builder->banks;
	adxShaderSignature->pushConstantCount = builder->pushConstantCount;
	if (!adxShaderSignature->constructDescriptorTableAllocators())
		return agpu::shader_signature_ref();
	return result;
}

bool ADXShaderSignature::constructDescriptorTableAllocators()
{
	size_t samplerHeapSize = 0;
	size_t srvHeapSize = 0;

	descriptorAllocators.resize(banks.size());

	auto d3dDevice = deviceForDX->d3dDevice;

	for (size_t bankIndex = 0; bankIndex < banks.size(); ++bankIndex)
	{
		auto &bank = banks[bankIndex];
		auto &heapSize = bank.isSamplersBank ? samplerHeapSize : srvHeapSize;

		// Setup the descriptor allocator.
		descriptorAllocators[bankIndex].setup(bank.maxBindings);
		
		heapSize += bank.descriptorTableSize * (bank.maxBindings + 1);
	}

	// Allocate sampler heap.
	if (samplerHeapSize > 0)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = samplerHeapSize / deviceForDX->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		if (FAILED(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&samplerHeap))))
			return nullptr;
	}

	// Allocate shader resource view description heap.
	if (srvHeapSize > 0)
	{
		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.NumDescriptors = srvHeapSize / deviceForDX->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

		if (FAILED(d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&shaderResourceViewHeap))))
			return nullptr;
	}

	samplerHeapSize = 0;
	srvHeapSize = 0;
	descriptorTableHeapGPUBaseAddress.resize(banks.size());
	descriptorTableHeapCPUBaseAddress.resize(banks.size());
	nullDescriptorTables.resize(banks.size());
	for (size_t bankIndex = 0; bankIndex < banks.size(); ++bankIndex)
	{
		auto& bank = banks[bankIndex];
		auto& heapSize = bank.isSamplersBank ? samplerHeapSize : srvHeapSize;
		auto& heap = bank.isSamplersBank ? samplerHeap : shaderResourceViewHeap;

		// Null descriptor
		{
			auto gpuAddress = heap->GetGPUDescriptorHandleForHeapStart();
			gpuAddress.ptr += heapSize;

			auto cpuAddress = heap->GetCPUDescriptorHandleForHeapStart();
			cpuAddress.ptr += heapSize;
			if (!initializeDescriptorTable(bank, cpuAddress))
				return false;

			nullDescriptorTables[bankIndex] = gpuAddress;
			heapSize += bank.descriptorTableSize;
		}
		
		// User specified descriptors.
		{
			auto gpuAddress = heap->GetGPUDescriptorHandleForHeapStart();
			gpuAddress.ptr += heapSize;

			auto cpuAddress = heap->GetCPUDescriptorHandleForHeapStart();
			cpuAddress.ptr += heapSize;

			descriptorTableHeapGPUBaseAddress[bankIndex] = gpuAddress;
			descriptorTableHeapCPUBaseAddress[bankIndex] = cpuAddress;
		}
		heapSize += bank.descriptorTableSize * bank.maxBindings;
	}

	return true;
}

bool ADXShaderSignature::initializeDescriptorTable(const ShaderSignatureBindingBank& bank, const D3D12_CPU_DESCRIPTOR_HANDLE& tableHandle)
{
	auto descriptorHandle = tableHandle;
	auto d3dDevice = deviceForDX->d3dDevice;

	if (bank.isSamplersBank)
	{
		D3D12_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

		for (size_t i = 0; i < bank.totalBindingPointCount; ++i)
		{
			d3dDevice->CreateSampler(&sampler, descriptorHandle);
		}

		return true;
	}

	for (size_t i = 0; i < bank.elements.size(); ++i)
	{
		auto &element = bank.elements[i];
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Shader4ComponentMapping = D3D12_ENCODE_SHADER_4_COMPONENT_MAPPING(D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0, D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0, D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_0, D3D12_SHADER_COMPONENT_MAPPING_FORCE_VALUE_1);
		
		switch (element.type)
		{
		case AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE:
		case AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE:
			desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
			break;
		case AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER:
		case AGPU_SHADER_BINDING_TYPE_STORAGE_BUFFER:
			desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
			break;
		default: abort();
		}

		for (size_t j = 0; j < element.arrayBindingPointCount; ++j)
		{
			d3dDevice->CreateShaderResourceView(nullptr, &desc, descriptorHandle);
			descriptorHandle.ptr += bank.descriptorSize;
		}
	}

	return true;
}

agpu::shader_resource_binding_ptr ADXShaderSignature::createShaderResourceBinding(agpu_uint bankIndex)
{
    std::unique_lock<std::mutex> l(allocationMutex);
    if (bankIndex >= banks.size())
        return nullptr;

	const auto& bank = banks[bankIndex];

    auto &allocator = descriptorAllocators[bankIndex];
	auto tableIndex = allocator.allocate();
    if (tableIndex < 0)
        return nullptr;

    UINT descriptorOffset = bank.descriptorTableSize * tableIndex;
	auto gpuHandle = descriptorTableHeapGPUBaseAddress[bankIndex];
	gpuHandle.ptr += descriptorOffset;

	auto cpuHandle = descriptorTableHeapCPUBaseAddress[bankIndex];
	cpuHandle.ptr += descriptorOffset;

	if (!initializeDescriptorTable(bank, cpuHandle))
		return nullptr;
	
	return ADXShaderResourceBinding::create(device, refFromThis<agpu::shader_signature> (), bankIndex, cpuHandle, gpuHandle).disown();
}

} // End of namespace AgpuD3D12
