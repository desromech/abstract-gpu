#include "shader_resource_binding.hpp"
#include "shader_signature.hpp"
#include "buffer.hpp"
#include "texture.hpp"
#include "texture_view.hpp"
#include "sampler.hpp"

namespace AgpuD3D12
{

ADXShaderResourceBinding::ADXShaderResourceBinding(const agpu::device_ref &cdevice, const agpu::shader_signature_ref &csignature)
    : device(cdevice), signature(csignature), bankIndex(0), cpuDescriptorTableHandle(), gpuDescriptorTableHandle()
{
}

ADXShaderResourceBinding::~ADXShaderResourceBinding()
{
}

agpu::shader_resource_binding_ref ADXShaderResourceBinding::create(const agpu::device_ref &device, const agpu::shader_signature_ref &signature, agpu_uint bankIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
{
    auto resourceBinding = agpu::makeObject<ADXShaderResourceBinding> (device, signature);
    auto adxResourceBinding = resourceBinding.as<ADXShaderResourceBinding> ();
    adxResourceBinding->bankIndex = bankIndex;
	adxResourceBinding->cpuDescriptorTableHandle = cpuHandle;
	adxResourceBinding->gpuDescriptorTableHandle = gpuHandle;
    return resourceBinding;
}

agpu_error ADXShaderResourceBinding::bindUniformBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer)
{
	CHECK_POINTER(uniform_buffer);

	auto adxBuffer = uniform_buffer.as<ADXBuffer> ();
    return bindUniformBufferRange(location, uniform_buffer, 0, adxBuffer->description.size);
}

agpu_error ADXShaderResourceBinding::bindUniformBufferRange(agpu_int location, const agpu::buffer_ref & uniform_buffer, agpu_size offset, agpu_size size)
{
	CHECK_POINTER(uniform_buffer);
	if (location < 0)
		return AGPU_OK;

	auto& bank = signature.as<ADXShaderSignature>()->banks[bankIndex];
	if (size_t(location) >= bank.elements.size())
		return AGPU_OUT_OF_BOUNDS;

	auto& element = bank.elements[location];
	auto descriptorCpuHandle = cpuDescriptorTableHandle;
	descriptorCpuHandle.ptr += element.firstDescriptorOffset;

	D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};
	auto error = uniform_buffer.as<ADXBuffer>()->createConstantBufferViewDescription(&viewDesc, offset, size);
	if (error) return error;

	deviceForDX->d3dDevice->CreateConstantBufferView(&viewDesc, descriptorCpuHandle);

	return AGPU_OK;
}

agpu_error ADXShaderResourceBinding::bindStorageBuffer(agpu_int location, const agpu::buffer_ref & storage_buffer)
{
	CHECK_POINTER(storage_buffer);

	auto adxBuffer = storage_buffer.as<ADXBuffer>();
	return bindStorageBufferRange(location, storage_buffer, 0, adxBuffer->description.size);
}

agpu_error ADXShaderResourceBinding::bindStorageBufferRange(agpu_int location, const agpu::buffer_ref & storage_buffer, agpu_size offset, agpu_size size)
{
	CHECK_POINTER(storage_buffer);
	if (location < 0)
		return AGPU_OK;

	auto& bank = signature.as<ADXShaderSignature>()->banks[bankIndex];
	if (size_t(location) >= bank.elements.size())
		return AGPU_OUT_OF_BOUNDS;

	auto& element = bank.elements[location];
	auto descriptorCpuHandle = cpuDescriptorTableHandle;
	descriptorCpuHandle.ptr += element.firstDescriptorOffset;

	D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};
	auto adxBuffer = storage_buffer.as<ADXBuffer> ();
	auto error = adxBuffer->createUAVDescription(&viewDesc, offset, size);
	if (error) return error;

	deviceForDX->d3dDevice->CreateUnorderedAccessView(adxBuffer->resource.Get(), nullptr, &viewDesc, descriptorCpuHandle);

	return AGPU_OK;
}

agpu_error ADXShaderResourceBinding::bindSampledTextureView(agpu_int location, const agpu::texture_view_ref & view)
{
	CHECK_POINTER(view);
	if (location < 0)
		return AGPU_OK;

	auto& bank = signature.as<ADXShaderSignature>()->banks[bankIndex];
	if (size_t(location) >= bank.elements.size())
		return AGPU_OUT_OF_BOUNDS;

	auto& element = bank.elements[location];
	auto descriptorCpuHandle = cpuDescriptorTableHandle;
	descriptorCpuHandle.ptr += element.firstDescriptorOffset;

	auto adxTextureView = view.as<ADXTextureView>();
	
	D3D12_SHADER_RESOURCE_VIEW_DESC viewDesc;
	auto error = adxTextureView->getSampledTextureViewDescription(&viewDesc);
	if (error) return error;
	
	deviceForDX->d3dDevice->CreateShaderResourceView(adxTextureView->texture.lock().as<ADXTexture> ()->resource.Get(), &viewDesc, descriptorCpuHandle);

	return AGPU_OK;
}

agpu_error ADXShaderResourceBinding::bindStorageImageView(agpu_int location, const agpu::texture_view_ref & view)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXShaderResourceBinding::bindSampler(agpu_int location, const agpu::sampler_ref & sampler)
{
	CHECK_POINTER(sampler);
	if (location < 0)
		return AGPU_OK;

	auto& bank = signature.as<ADXShaderSignature>()->banks[bankIndex];
	if (size_t(location) >= bank.elements.size())
		return AGPU_OUT_OF_BOUNDS;

	auto& element = bank.elements[location];
	auto descriptorCpuHandle = cpuDescriptorTableHandle;
	descriptorCpuHandle.ptr += element.firstDescriptorOffset;

	D3D12_SAMPLER_DESC samplerDesc;
	auto error = sampler.as<ADXSampler>()->getSamplerDesc(&samplerDesc);
	if (error) return error;

	deviceForDX->d3dDevice->CreateSampler(&samplerDesc, descriptorCpuHandle);

    return AGPU_OK;
}
/*
agpu_error _agpu_shader_resource_binding::createSampler(agpu_int location, agpu_sampler_description* description)
{
    CHECK_POINTER(description);

    std::unique_lock<std::mutex>(bindMutex);
    if (type != AGPU_SHADER_BINDING_TYPE_SAMPLER)
        return AGPU_INVALID_OPERATION;

    if (location < 0)
        return AGPU_OK;
    if (location >= samplerCount)
        return AGPU_OUT_OF_BOUNDS;

    // Create the sampler description
    D3D12_SAMPLER_DESC sampler;
    memset(&sampler, 0, sizeof(sampler));
    sampler.Filter = (D3D12_FILTER)description->filter;
    sampler.AddressU = (D3D12_TEXTURE_ADDRESS_MODE)description->address_u;
    sampler.AddressV = (D3D12_TEXTURE_ADDRESS_MODE)description->address_v;
    sampler.AddressW = (D3D12_TEXTURE_ADDRESS_MODE)description->address_w;
    sampler.MipLODBias = description->mip_lod_bias;
    sampler.MaxAnisotropy= description->maxanisotropy;
    sampler.BorderColor[0]= description->border_color.r;
    sampler.BorderColor[1] = description->border_color.g;
    sampler.BorderColor[2] = description->border_color.b;
    sampler.BorderColor[3] = description->border_color.a;
    sampler.MinLOD = description->min_lod;
    sampler.MaxLOD = description->max_lod;

    // Set the descriptor.
    auto cpuHandle = signature->samplerHeap->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += descriptorOffset + location*signature->samplerDescriptorSize;
    deviceForDX->d3dDevice->CreateSampler(&sampler, cpuHandle);

    return AGPU_OK;
}
*/
} // End of namespace AgpuD3D12
