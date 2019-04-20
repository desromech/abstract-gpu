#ifndef AGPU_GL_SHADER_SIGNATURE_HPP
#define AGPU_GL_SHADER_SIGNATURE_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"

namespace AgpuGL
{

struct GLShaderSignature: public agpu::shader_signature
{
public:
    GLShaderSignature();
    ~GLShaderSignature();

    static agpu::shader_signature_ref create(const agpu::device_ref &device, const GLShaderSignatureBuilder *builder);

    virtual agpu::shader_resource_binding_ptr createShaderResourceBinding(agpu_uint element) override;

    int mapDescriptorSetAndBinding(agpu_shader_binding_type type, unsigned int set, unsigned int binding);

    agpu::device_ref device;
    std::vector<ShaderSignatureElement> elements;
    agpu_uint bindingPointsUsed[(int)OpenGLResourceBindingType::Count];
    agpu_uint uniformVariableCount;
};

} // End of namespace AgpuGL

#endif //AGPU_GL_SHADER_SIGNATURE_HPP
