#ifndef AGPU_GL_SHADER_RESOURCE_BINDING_HPP
#define AGPU_GL_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"
#include <vector>

namespace AgpuGL
{

struct BufferBinding
{
    BufferBinding()
        : range(false), offset(0), size(-1) {}

    agpu::buffer_ref buffer;
    bool range;
    size_t offset;
    size_t size;
};

struct TextureBinding
{
    TextureBinding() {}

    agpu::texture_ref texture;
    agpu_uint startMiplevel;
    agpu_int miplevels;
    agpu_float lodClamp;
};

struct GLShaderResourceBinding : public agpu::shader_resource_binding
{
public:
    GLShaderResourceBinding();
    ~GLShaderResourceBinding();

    static agpu::shader_resource_binding_ref create(const agpu::shader_signature_ref &signature, int elementIndex);

    virtual agpu_error bindUniformBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer) override;
    virtual agpu_error bindUniformBufferRange(agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size) override;
    virtual agpu_error bindStorageBuffer(agpu_int location, const agpu::buffer_ref &uniform_buffer) override;
    virtual agpu_error bindStorageBufferRange(agpu_int location, const agpu::buffer_ref &uniform_buffer, agpu_size offset, agpu_size size) override;
    virtual agpu_error bindTexture(agpu_int location, const agpu::texture_ref& texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp) override;
    virtual agpu_error bindTextureArrayRange(agpu_int location, const agpu::texture_ref &texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp) override;
    virtual agpu_error bindImage(agpu_int location, const agpu::texture_ref &texture, agpu_int level, agpu_int layer, agpu_mapping_access access, agpu_texture_format format) override;

    TextureBinding *getTextureBindingAt(agpu_int location);
    GLuint getSamplerAt(agpu_int location);

    agpu_error createSampler ( agpu_int location, agpu_sampler_description* description );


public:
    void activate();

    agpu::device_ref device;
    agpu::shader_signature_ref signature;
    int elementIndex;

private:

    void activateUniformBuffers();
    void activateStorageBuffers();
    void activateBuffers(GLenum target, size_t baseIndex, std::vector<BufferBinding> &buffers);

    void activateSampledImages();
    void activateSamplers();

    std::vector<BufferBinding> uniformBuffers;
    std::vector<BufferBinding> storageBuffers;
    std::vector<TextureBinding> sampledImages;
    std::vector<GLuint> samplers;
};

} // End of namespace AgpuGL

#endif //AGPU_GL_SHADER_RESOURCE_BINDING_HPP
