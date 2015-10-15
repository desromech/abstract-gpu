#ifndef AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"

struct ShaderSignatureElementDescription
{
    ShaderSignatureElementDescription() {}
    ShaderSignatureElementDescription(bool bank, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings)
        : valid(true), bank(bank), type(type), bindingPointCount(bindingPointCount), maxBindings(maxBindings) {}

    bool valid;
    bool bank;
    agpu_shader_binding_type type;
    agpu_uint bindingPointCount;
    agpu_uint maxBindings;
};

struct _agpu_shader_signature_builder : public Object<_agpu_shader_signature_builder>
{
public:
    _agpu_shader_signature_builder();

    void lostReferences();

    static agpu_shader_signature_builder *create(agpu_device *device);

    agpu_shader_signature* buildShaderSignature();
    agpu_error addBindingConstant();
    agpu_error addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings);
    agpu_error addBindingBank(agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings);

public:
    agpu_device *device;

    int elementCount;
    int rangeCount;
    D3D12_ROOT_SIGNATURE_DESC description;
    D3D12_ROOT_PARAMETER rootParameters[16];
    D3D12_DESCRIPTOR_RANGE ranges[16];
    agpu_uint baseRegisterCount[AGPU_SHADER_BINDING_TYPE_COUNT];
    ShaderSignatureElementDescription elementsDescription[16];
    agpu_uint maxBindingsCount[AGPU_SHADER_BINDING_TYPE_COUNT];
};

#endif //AGPU_D3D12_SHADER_SIGNATURE_BUILDER_HPP
