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

agpu_error ADXShaderResourceBinding::bindArrayOfSampledTextureView(agpu_int location, agpu_int first_index, agpu_uint count, agpu::texture_view_ref* views)
{
	return AGPU_UNSUPPORTED;
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

	D3D12_UNORDERED_ACCESS_VIEW_DESC viewDesc = {};
	auto error = adxTextureView->getUnorderedAccessTextureViewDescription(&viewDesc);
	if (error) return error;

	deviceForDX->d3dDevice->CreateUnorderedAccessView(adxTextureView->texture.lock().as<ADXTexture>()->resource.Get(), nullptr, &viewDesc, descriptorCpuHandle);
	return AGPU_OK;
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

} // End of namespace AgpuD3D12
