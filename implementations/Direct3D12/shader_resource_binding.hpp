#ifndef AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP
#define AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP

#include <vector>
#include "device.hpp"

namespace AgpuD3D12
{

class ADXShaderResourceBinding : public agpu::shader_resource_binding
{
public:
    ADXShaderResourceBinding(const agpu::device_ref &cdevice, const agpu::shader_signature_ref &signature);
    ~ADXShaderResourceBinding();

    static agpu::shader_resource_binding_ref create(const agpu::device_ref &device, const agpu::shader_signature_ref &signature, agpu_uint bankIndex, D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

    virtual agpu_error bindUniformBuffer(agpu_int location, const agpu::buffer_ref & uniform_buffer) override;
	virtual agpu_error bindUniformBufferRange(agpu_int location, const agpu::buffer_ref & uniform_buffer, agpu_size offset, agpu_size size) override;
	virtual agpu_error bindStorageBuffer(agpu_int location, const agpu::buffer_ref & storage_buffer) override;
	virtual agpu_error bindStorageBufferRange(agpu_int location, const agpu::buffer_ref & storage_buffer, agpu_size offset, agpu_size size) override;
	virtual agpu_error bindSampledTextureView(agpu_int location, const agpu::texture_view_ref & view) override;
	virtual agpu_error bindArrayOfSampledTextureView(agpu_int location, agpu_int first_index, agpu_uint count, agpu::texture_view_ref* views) override;
	virtual agpu_error bindStorageImageView(agpu_int location, const agpu::texture_view_ref & view) override;
	virtual agpu_error bindSampler(agpu_int location, const agpu::sampler_ref & sampler) override;

public:
    agpu::device_ref device;
    agpu::shader_signature_ref signature;
    agpu_uint bankIndex;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorTableHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorTableHandle;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_SHADER_RESOURCE_BINDING_HPP
