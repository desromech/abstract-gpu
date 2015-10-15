#include <string.h>
#include "shader_signature_builder.hpp"
#include "shader_signature.hpp"

inline D3D12_DESCRIPTOR_RANGE_TYPE mapBindingType(agpu_shader_binding_type type)
{
    switch (type)
    {
    case AGPU_SHADER_BINDING_TYPE_SRV: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    case AGPU_SHADER_BINDING_TYPE_UAV: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
    case AGPU_SHADER_BINDING_TYPE_CBV: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
    case AGPU_SHADER_BINDING_TYPE_SAMPLER: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    default: abort();
    }
}

inline D3D12_ROOT_PARAMETER_TYPE mapParameterType(agpu_shader_binding_type type)
{
    switch (type)
    {
    case AGPU_SHADER_BINDING_TYPE_SRV: return D3D12_ROOT_PARAMETER_TYPE_SRV;
    case AGPU_SHADER_BINDING_TYPE_UAV: return D3D12_ROOT_PARAMETER_TYPE_UAV;
    case AGPU_SHADER_BINDING_TYPE_CBV: return D3D12_ROOT_PARAMETER_TYPE_CBV;
    default: abort();
    }
}


_agpu_shader_signature_builder::_agpu_shader_signature_builder()
{
    elementCount = 0;
    rangeCount = 0;
    memset(&description, 0, sizeof(description));
    memset(rootParameters, 0, sizeof(rootParameters));
    memset(baseRegisterCount, 0, sizeof(baseRegisterCount));
    memset(elementsDescription, 0, sizeof(elementsDescription));
    memset(maxBindingsCount, 0, sizeof(maxBindingsCount));
    description.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    description.pParameters = rootParameters;
}

void _agpu_shader_signature_builder::lostReferences()
{
}

agpu_shader_signature_builder *_agpu_shader_signature_builder::create(agpu_device *device)
{
    std::unique_ptr<agpu_shader_signature_builder> builder(new agpu_shader_signature_builder);
    builder->device = device;
    return builder.release();
}

agpu_shader_signature* _agpu_shader_signature_builder::buildShaderSignature()
{
    description.NumParameters = elementCount;

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    if (FAILED(D3D12SerializeRootSignature(&description, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error)))
    {
        printError("Failed to create the root signature: %s\n", (const char*)error->GetBufferPointer());
        return nullptr;
    }

    ComPtr<ID3D12RootSignature> rootSignature;
    if (FAILED(device->d3dDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
        return nullptr;

    return agpu_shader_signature::create(device, rootSignature, elementsDescription, maxBindingsCount);
}

agpu_error _agpu_shader_signature_builder::addBindingConstant()
{
    if (elementCount == 16)
        return AGPU_INVALID_OPERATION;

    auto &param = rootParameters[elementCount++];
    return AGPU_UNIMPLEMENTED;

}

agpu_error _agpu_shader_signature_builder::addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings)
{
    if (elementCount == 16)
        return AGPU_INVALID_OPERATION;

    elementsDescription[elementCount] = ShaderSignatureElementDescription(false, type, 1, maxBindings);
    auto &param = rootParameters[elementCount++];
    param.ParameterType = mapParameterType(type);
    param.Descriptor.ShaderRegister = baseRegisterCount[type];
    param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
    ++baseRegisterCount[type];
    maxBindingsCount[type] += maxBindings;

    return AGPU_OK;
}

agpu_error _agpu_shader_signature_builder::addBindingBank(agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings)
{
    if (elementCount == 16)
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
    param.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

    return AGPU_OK;
}

// Exported C interface
AGPU_EXPORT agpu_error agpuAddShaderSignatureBuilderReference(agpu_shader_signature_builder* shader_signature_builder)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->retain();
}

AGPU_EXPORT agpu_error agpuReleaseShaderSignatureBuilder(agpu_shader_signature_builder* shader_signature_builder)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->release();
}

AGPU_EXPORT agpu_shader_signature* agpuBuildShaderSignature(agpu_shader_signature_builder* shader_signature_builder)
{
    if (!shader_signature_builder)
        return nullptr;
    return shader_signature_builder->buildShaderSignature();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingConstant(agpu_shader_signature_builder* shader_signature_builder)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingConstant();
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingElement(agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint maxBindings)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingElement(type, maxBindings);
}

AGPU_EXPORT agpu_error agpuAddShaderSignatureBindingBank(agpu_shader_signature_builder* shader_signature_builder, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings)
{
    CHECK_POINTER(shader_signature_builder);
    return shader_signature_builder->addBindingBank(type, bindingPointCount, maxBindings);
}
