#include <string.h>
#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

namespace AgpuD3D12
{

inline D3D12_DESCRIPTOR_RANGE_TYPE mapBindingType(agpu_shader_binding_type type)
{
    switch (type)
    {
    case AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    case AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    case AGPU_SHADER_BINDING_TYPE_SAMPLER: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    default: abort();
    }
}

inline D3D12_ROOT_PARAMETER_TYPE mapParameterType(agpu_shader_binding_type type)
{
    switch (type)
    {
    case AGPU_SHADER_BINDING_TYPE_SAMPLED_IMAGE: return D3D12_ROOT_PARAMETER_TYPE_SRV;
    case AGPU_SHADER_BINDING_TYPE_STORAGE_IMAGE: return D3D12_ROOT_PARAMETER_TYPE_UAV;
    case AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER: return D3D12_ROOT_PARAMETER_TYPE_CBV;
    default: abort();
    }
}


ShaderSignatureBindingBankElement::ShaderSignatureBindingBankElement()
	: type(AGPU_SHADER_BINDING_TYPE_UNIFORM_BUFFER), bindingPointCount(0)
{
}

ShaderSignatureBindingBankElement::~ShaderSignatureBindingBankElement()
{
}

ShaderSignatureBindingBank::ShaderSignatureBindingBank()
	: maxBindings(0)
{
}

ShaderSignatureBindingBank::~ShaderSignatureBindingBank()
{
}

ADXShaderSignatureBuilder::ADXShaderSignatureBuilder(const agpu::device_ref &cdevice)
    : device(cdevice), pushConstantCount(0)
{
}

ADXShaderSignatureBuilder::~ADXShaderSignatureBuilder()
{
}

agpu::shader_signature_builder_ref ADXShaderSignatureBuilder::create(const agpu::device_ref &device)
{
    return agpu::makeObject<ADXShaderSignatureBuilder> (device);
}

agpu::shader_signature_ptr ADXShaderSignatureBuilder::build()
{
	D3D12_ROOT_SIGNATURE_DESC rootDescription = {};
	rootDescription.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	std::vector<D3D12_ROOT_PARAMETER> rootParameters;
	rootParameters.reserve(banks.size());
	for (auto& bank : banks)
	{
		D3D12_ROOT_PARAMETER parameter = {};
		parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

		if (!bank.descriptorRanges.empty())
		{
			parameter.DescriptorTable.NumDescriptorRanges = bank.descriptorRanges.size();
			parameter.DescriptorTable.pDescriptorRanges = &bank.descriptorRanges[0];
		}
	}

	if (pushConstantCount > 0)
	{
		D3D12_ROOT_PARAMETER parameter = {};
		parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		parameter.Constants.Num32BitValues = pushConstantCount;
		rootParameters.push_back(parameter);
	}

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeRootSignature(&rootDescription, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
    {
        printError("Failed to create the root signature: %s\n", (const char*)error->GetBufferPointer());
        return nullptr;
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    if (FAILED(deviceForDX->d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
        return nullptr;

    return ADXShaderSignature::create(device, rootSignature, this).disown();
}

agpu_error ADXShaderSignatureBuilder::addBindingConstant()
{
	++pushConstantCount;
    return AGPU_OK;
}

agpu_error ADXShaderSignatureBuilder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error ADXShaderSignatureBuilder::beginBindingBank(agpu_uint maxBindings)
{
	ShaderSignatureBindingBank bank;
	bank.maxBindings = maxBindings;
	banks.push_back(bank);

    /*if (elementCount == 16)
        return AGPU_INVALID_OPERATION;
    if (rangeCount == 16)
        return AGPU_INVALID_OPERATION;

    elementsDescription[elementCount] = ShaderSignatureElementDescription(true, type, bindingPointCount, maxBindings);
    auto &param = rootParameters[elementCount++];
    auto &range = ranges[rangeCount++];
    range.BaseShaderRegister = baseRegisterCount[type];
    range.NumDescriptors = bindingPointCount;
    range.RangeType = mapBindingType(type);
    range.RegisterSpace = 0;
    range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
    baseRegisterCount[type] += bindingPointCount;
    maxBindingsCount[type] += bindingPointCount*maxBindings;

    param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    param.DescriptorTable.NumDescriptorRanges = 1;
    param.DescriptorTable.pDescriptorRanges = &range;
    param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;*/

    return AGPU_OK;
}

agpu_error ADXShaderSignatureBuilder::addBindingBankElement(agpu_shader_binding_type type, agpu_uint bindingPointCount)
{
	if (banks.empty())
		return AGPU_INVALID_OPERATION;

    return AGPU_UNIMPLEMENTED;
}

} // End of namespace AgpuD3D12
