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

class GLAbstractTextureView;

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
	virtual agpu_error bindSampledTextureView(agpu_int location, const agpu::texture_view_ref & view) override;
	virtual agpu_error bindStorageImageView(agpu_int location, const agpu::texture_view_ref & view) override;
	virtual agpu_error bindSampler(agpu_int location, const agpu::sampler_ref & sampler) override;

    GLAbstractTextureView *getTextureBindingAt(agpu_int location);

    GLuint getSamplerAt(agpu_int location);

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
    std::vector<agpu::texture_view_ref> sampledTextures;
    std::vector<agpu::texture_view_ref> storageTextures;
    std::vector<agpu::sampler_ref> samplers;
};

} // End of namespace AgpuGL

#endif //AGPU_GL_SHADER_RESOURCE_BINDING_HPP
