#ifndef AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"
#include <vector>

namespace AgpuD3D12
{

class ShaderSignatureBindingBankElement
{
public:
	ShaderSignatureBindingBankElement();
	~ShaderSignatureBindingBankElement();

    agpu_shader_binding_type type;
	agpu_uint baseDescriptorIndex;
    agpu_uint arrayBindingPointCount;
	size_t firstDescriptorOffset;
	size_t descriptorRangeSize;
};

class ShaderSignatureBindingBank
{
public:
	ShaderSignatureBindingBank();
	~ShaderSignatureBindingBank();
	
	agpu_uint maxBindings;
	agpu_uint totalBindingPointCount;
	bool isSamplersBank;
	size_t descriptorSize;
	size_t descriptorTableSize;
	std::vector<ShaderSignatureBindingBankElement> elements;
	std::vector<D3D12_DESCRIPTOR_RANGE> descriptorRanges;
};

class ADXShaderSignatureBuilder : public agpu::shader_signature_builder
{
public:
	ADXShaderSignatureBuilder(const agpu::device_ref& cdevice);
	~ADXShaderSignatureBuilder();

	static agpu::shader_signature_builder_ref create(const agpu::device_ref& cdevice);

	virtual agpu::shader_signature_ptr build() override;
	virtual agpu_error addBindingConstant() override;
	virtual agpu_error addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings) override;
	virtual agpu_error beginBindingBank(agpu_uint maxBindings) override;
	virtual agpu_error addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount) override;
	agpu_error addBindingBankArrayElement(agpu_shader_binding_type type, agpu_uint maxBindings, agpu_uint arraySize);

public:
	agpu::device_ref device;

	std::vector<ShaderSignatureBindingBank> banks;
	size_t pushConstantCount;
};

} // End of namespace AgpuD3D12


#endif //AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
