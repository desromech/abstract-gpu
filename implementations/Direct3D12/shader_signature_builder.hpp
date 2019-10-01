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
    agpu_uint bindingPointCount;
};

class ShaderSignatureBindingBank
{
public:
	ShaderSignatureBindingBank();
	~ShaderSignatureBindingBank();
	
	agpu_uint maxBindings;
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

public:
	agpu::device_ref device;

	std::vector<ShaderSignatureBindingBank> banks;
	size_t pushConstantCount;

	//agpu_uint baseRegisterCount[AGPU_SHADER_BINDING_TYPE_COUNT];
    //agpu_uint maxBindingsCount[AGPU_SHADER_BINDING_TYPE_COUNT];
};

} // End of namespace AgpuD3D12


#endif //AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
