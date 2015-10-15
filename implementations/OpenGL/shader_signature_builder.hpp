#ifndef AGPU_GL_SHADER_SIGNATURE_BUILDER_HPP
#define AGPU_GL_SHADER_SIGNATURE_BUILDER_HPP

#include "device.hpp"

struct ShaderSignatureElementDescription
{
    ShaderSignatureElementDescription() {}
    ShaderSignatureElementDescription(bool bank, agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings, agpu_uint startIndex)
        : valid(true), bank(bank), type(type), bindingPointCount(bindingPointCount), maxBindings(maxBindings), startIndex(startIndex) {}

    bool valid;
    bool bank;
    agpu_shader_binding_type type;
    agpu_uint bindingPointCount;
    agpu_uint maxBindings;
    agpu_uint startIndex;
};

struct _agpu_shader_signature_builder: public Object<_agpu_shader_signature_builder>
{
public:
    _agpu_shader_signature_builder();

    void lostReferences();

    static _agpu_shader_signature_builder *create(agpu_device *device);

    agpu_shader_signature* build();
    agpu_error addBindingConstant();
    agpu_error addBindingElement(agpu_shader_binding_type type, agpu_uint maxBindings);
    agpu_error addBindingBank(agpu_shader_binding_type type, agpu_uint bindingPointCount, agpu_uint maxBindings);

public:
    agpu_device *device;

private:
    int elementCount;
    ShaderSignatureElementDescription elements[16];
    agpu_uint bindingCount[AGPU_SHADER_BINDING_TYPE_COUNT];
};

#endif //AGPU_GL_SHADER_SIGNATURE_BUILDER_HPP
