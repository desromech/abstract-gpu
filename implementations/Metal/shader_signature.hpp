#ifndef AGPU_METAL_SHADER_SIGNATURE_HPP
#define AGPU_METAL_SHADER_SIGNATURE_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"
#include "spirv_msl.hpp"

namespace AgpuMetal
{
    
struct AMtlShaderSignature : public agpu::shader_signature
{
public:
    AMtlShaderSignature(const agpu::device_ref &device);
    ~AMtlShaderSignature();

    static agpu::shader_signature_ref create(const agpu::device_ref &device, AMtlShaderSignatureBuilder *builder);

    virtual agpu::shader_resource_binding_ptr createShaderResourceBinding(agpu_uint element) override;
    
    int mapDescriptorSetAndBinding(agpu_shader_binding_type type, unsigned int set, unsigned int binding);
    void buildMSLMapping();

    agpu::device_ref device;
    std::vector<ShaderSignatureElement> elements;

    agpu_size pushConstantBufferSize;
    agpu_uint pushConstantBufferIndex;
    agpu_uint boundVertexBufferCount;
    
    std::vector<spirv_cross::MSLResourceBinding> resourceBindings;
};

} // End of namespace AgpuMetal

#endif // AGPU_METAL_SHADER_SIGNATURE_HPP
