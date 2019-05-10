#ifndef AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
#define AGPU_METAL_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"
#include <vector>

namespace AgpuMetal
{
    
struct BufferBinding
{
    BufferBinding()
        : offset(0), size(0) {}
    ~BufferBinding();

    void reset();

    agpu::buffer_ref buffer;
    agpu_size offset;
    agpu_size size;
};

struct AMtlShaderResourceBinding : public agpu::shader_resource_binding
{
public:
    AMtlShaderResourceBinding(const agpu::device_ref &device);
    ~AMtlShaderResourceBinding();

    static agpu::shader_resource_binding_ref create(const agpu::device_ref &device, const agpu::shader_signature_ref &signature, agpu_uint elementIndex);

    virtual agpu_error bindUniformBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer) override;
    virtual agpu_error bindUniformBufferRange( agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size) override;
    virtual agpu_error bindStorageBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer) override;
    virtual agpu_error bindStorageBufferRange(agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size) override;
    virtual agpu_error bindTexture(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp) override;
    virtual agpu_error bindTextureArrayRange( agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp) override;
    virtual agpu_error bindImage(agpu_int location, const agpu::texture_ref &texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format) override;
    virtual agpu_error createSampler(agpu_int location, agpu_sampler_description* description) override;

    agpu_error activateOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder);
    agpu_error activateComputeOn(id<MTLComputeCommandEncoder> encoder);

    agpu::device_ref device;
    agpu::shader_signature_ref signature;
    agpu_uint elementIndex;

    std::vector<BufferBinding> buffers;
    std::vector<agpu::texture_ref> textures;
    std::vector<id<MTLSamplerState> > samplers;

private:
    agpu_error activateBuffersOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder);
    agpu_error activateSamplersOn(id<MTLRenderCommandEncoder> encoder);
    agpu_error activateTexturesOn(id<MTLRenderCommandEncoder> encoder);

    agpu_error activateComputeBuffersOn(id<MTLComputeCommandEncoder> encoder);
    agpu_error activateComputeSamplersOn(id<MTLComputeCommandEncoder> encoder);
    agpu_error activateComputeTexturesOn(id<MTLComputeCommandEncoder> encoder);

};

} // End of namespace AgpuMetal

#endif // AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
