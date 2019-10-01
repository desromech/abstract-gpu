#include <algorithm>
#include "shader_signature.hpp"
#include "shader_resource_binding.hpp"

namespace AgpuD3D12
{

DescriptorAllocator::DescriptorAllocator(int startIndex, int descriptorCount)
    : startIndex(startIndex), descriptorCount(descriptorCount)
{
    freeCount = descriptorCount;
    freeList.resize(descriptorCount);
    for (int i = 0; i < freeCount; ++i) {
        freeList[i] = i;
    }
}

DescriptorAllocator::~DescriptorAllocator()
{
}

void DescriptorAllocator::free(int descriptorIndex)
{
    freeList[freeCount++] = descriptorIndex;
}

int DescriptorAllocator::getStartIndex()
{
    return startIndex;
}

int DescriptorAllocator::allocate()
{
    if (!freeCount)
        return -1;
    return freeList[--freeCount];
}

ADXShaderSignature::ADXShaderSignature(const agpu::device_ref &cdevice)
    : device(cdevice)
{
}

ADXShaderSignature::~ADXShaderSignature()
{
}

agpu::shader_signature_ref ADXShaderSignature::create(const agpu::device_ref &device, const ComPtr<ID3D12RootSignature> &rootSignature, ADXShaderSignatureBuilder* builder)
{
	auto result = agpu::makeObject<ADXShaderSignature> (device);
	auto adxShaderSignature = result.as<ADXShaderSignature> ();
	adxShaderSignature->rootSignature = rootSignature;
	adxShaderSignature->banks = builder->banks;
	adxShaderSignature->pushConstantCount = builder->pushConstantCount;
    return result;
    /*UINT maxSrvDescriptorCount = 0;
    UINT maxSamplerDescriptorCount = maxBindingsCount[AGPU_SHADER_BINDING_TYPE_SAMPLER];
    for (int i = 0; i < AGPU_SHADER_BINDING_TYPE_COUNT; ++i)
    {
        if (i != AGPU_SHADER_BINDING_TYPE_SAMPLER)
            maxSrvDescriptorCount += maxBindingsCount[i];
    }

    // Copy the parameters
    std::unique_ptr<ADXShaderSignature> signature(new ADXShaderSignature());
    signature->device = device;
    signature->rootSignature = rootSignature;
    memcpy(signature->elementsDescription, elementsDescription, sizeof(elementsDescription[0])*16);
    memcpy(signature->maxBindingsCount, maxBindingsCount, sizeof(maxBindingsCount[0])*AGPU_SHADER_BINDING_TYPE_COUNT);

    // Get descriptor sizes
    signature->shaderResourceViewDescriptorSize = deviceForDX->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    signature->samplerDescriptorSize = deviceForDX->d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

    agpu_uint maxDescriptorBindingPoints[AGPU_SHADER_BINDING_TYPE_COUNT];
    memset(maxDescriptorBindingPoints, 0, sizeof(maxDescriptorBindingPoints));
    for (int i = 0; i < 16; ++i)
    {
        auto &element = elementsDescription[i];
        if (!element.valid)
            continue;

        maxDescriptorBindingPoints[element.type] = std::max(maxDescriptorBindingPoints[element.type], element.bindingPointCount);
    }

    auto nullSrvDescriptorCount = 0;
    for (int i = 0; i < AGPU_SHADER_BINDING_TYPE_COUNT; ++i)
    {
        if (i != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        {
            agpu_uint multiplier = 1;
            if (i == AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE)
                multiplier = (agpu_uint)ShaderResourceViewType::Count;
            if (i == AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE)
                multiplier = (agpu_uint)UnorderedAccessViewType::Count;

            nullSrvDescriptorCount += maxDescriptorBindingPoints[i]* multiplier;
        }
    }

    auto nullSamplerDescriptorCount = maxDescriptorBindingPoints[AGPU_SHADER_BINDING_TYPE_SAMPLER];

    // Allocate shader resource view description heap.
    if(maxSrvDescriptorCount)
    {
        // Describe and create a constant buffer view descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = maxSrvDescriptorCount + nullSrvDescriptorCount;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        if (FAILED(deviceForDX->d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&signature->shaderResourceViewHeap))))
            return nullptr;
    }

    // Allocate sampler heap.
    if(maxSamplerDescriptorCount)
    {
        // Describe and create a constant buffer view descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
        heapDesc.NumDescriptors = maxSamplerDescriptorCount + nullSamplerDescriptorCount;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        if (FAILED(deviceForDX->d3dDevice->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&signature->samplerHeap))))
            return nullptr;
    }

    // Create the bank allocators
    int samplerDescriptorCount = 0;
    int srvDescriptorCount = 0;

    for (int i = 0; i < 16; ++i)
    {
        auto &element = elementsDescription[i];
        if (!element.valid)
            continue;


        if (element.type == AGPU_SHADER_BINDING_TYPE_SAMPLER)
        {
            signature->descriptorAllocators[i] = new DescriptorAllocator(samplerDescriptorCount, element.maxBindings);
            samplerDescriptorCount += element.bindingPointCount*element.maxBindings;
        }
        else
        {
            signature->descriptorAllocators[i] = new DescriptorAllocator(srvDescriptorCount, element.maxBindings);
            srvDescriptorCount += element.bindingPointCount*element.maxBindings;
        }
    }

    assert(samplerDescriptorCount == maxSamplerDescriptorCount);
    assert(srvDescriptorCount == maxSrvDescriptorCount);

    signature->shaderResourceViewDescriptorReservedSize = maxSrvDescriptorCount * signature->shaderResourceViewDescriptorSize;
    signature->samplerDescriptorReservedSize = maxSamplerDescriptorCount * signature->samplerDescriptorSize;
    UINT currentOffset = signature->shaderResourceViewDescriptorReservedSize;

    // Create the null descriptors
    signature->nullCbvDescriptorOffset = currentOffset;
    for (agpu_uint i = 0; i < maxDescriptorBindingPoints[AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER]; ++i)
    {
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
        memset(&cbvDesc, 0, sizeof(cbvDesc));

        auto cpuHandle = signature->shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
        cpuHandle.ptr += currentOffset;

        deviceForDX->d3dDevice->CreateConstantBufferView(&cbvDesc, cpuHandle);
        currentOffset += signature->shaderResourceViewDescriptorSize;
    }

    for (int t = 0; t < (int)UnorderedAccessViewType::Count; ++t)
    {
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc;
        memset(&uavDesc, 0, sizeof(uavDesc));
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION(t + D3D12_UAV_DIMENSION_BUFFER);
        uavDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

        signature->nullUavDescriptorOffset[t] = currentOffset;
        for (agpu_uint i = 0; i < maxDescriptorBindingPoints[AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE]; ++i)
        {
            auto cpuHandle = signature->shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
            cpuHandle.ptr += currentOffset;

            deviceForDX->d3dDevice->CreateUnorderedAccessView(nullptr, nullptr, &uavDesc, cpuHandle);
            currentOffset += signature->shaderResourceViewDescriptorSize;
        }

    }

    for (int t = 0; t < (int)ShaderResourceViewType::Count; ++t)
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc;
        memset(&srvDesc, 0, sizeof(srvDesc));
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION(t + D3D12_SRV_DIMENSION_BUFFER);
        srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        signature->nullSrvDescriptorOffset[t] = currentOffset;
        for (agpu_uint i = 0; i < maxDescriptorBindingPoints[AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE]; ++i)
        {
            auto cpuHandle = signature->shaderResourceViewHeap->GetCPUDescriptorHandleForHeapStart();
            cpuHandle.ptr += currentOffset;

            deviceForDX->d3dDevice->CreateShaderResourceView(nullptr, &srvDesc, cpuHandle);
            currentOffset += signature->shaderResourceViewDescriptorSize;
        }
    }

    return signature.release();*/
}

agpu::shader_resource_binding_ptr ADXShaderSignature::createShaderResourceBinding(agpu_uint element)
{
    return nullptr;
    /*std::unique_lock<std::mutex> l(allocationMutex);
    if (element >= 16)
        return nullptr;

    auto &elementDesc = elementsDescription[element];
    if (!elementDesc.valid)
        return nullptr;

    auto allocator = descriptorAllocators[element];
    auto index = allocator->allocate();
    auto startIndex = allocator->getStartIndex();
    if (index < 0)
        return nullptr;

    UINT descriptorOffset;
    if(elementDesc.type == AGPU_SHADER_BINDING_TYPE_SAMPLER)
        descriptorOffset = (startIndex + index*elementDesc.bindingPointCount)*samplerDescriptorSize;
    else
        descriptorOffset = (startIndex + index*elementDesc.bindingPointCount)*shaderResourceViewDescriptorSize;

    return agpu_shader_resource_binding::create(this, element, descriptorOffset);*/
}

} // End of namespace AgpuD3D12
