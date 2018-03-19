#ifndef AGPU_GL_SHADER_RESOURCE_BINDING_HPP
#define AGPU_GL_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"
#include <vector>

struct BufferBinding
{
    BufferBinding()
        : buffer(nullptr), range(false), offset(0), size(-1) {}

    agpu_buffer *buffer;
    bool range;
    size_t offset;
    size_t size;
};

struct TextureBinding
{
    TextureBinding()
        : texture(nullptr) {}

    agpu_texture *texture;
    agpu_uint startMiplevel;
    agpu_int miplevels;
    agpu_float lodClamp;
};

struct _agpu_shader_resource_binding : public Object<_agpu_shader_resource_binding>
{
public:
    _agpu_shader_resource_binding();

    void lostReferences();

    static agpu_shader_resource_binding *create(agpu_shader_signature *signature, int elementIndex);

    agpu_error bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer );
    agpu_error bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
    agpu_error bindStorageBuffer ( agpu_int location, agpu_buffer* uniform_buffer );
    agpu_error bindStorageBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
    agpu_error bindTexture ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp );
    agpu_error bindTextureArrayRange ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp );

    TextureBinding *getTextureBindingAt(agpu_int location);
    GLuint getSamplerAt(agpu_int location);

    agpu_error createSampler ( agpu_int location, agpu_sampler_description* description );


public:
    void activate();

    agpu_device *device;
    agpu_shader_signature *signature;
    int elementIndex;

private:

    void activateUniformBuffers();
    void activateStorageBuffers();
    void activateBuffers(GLenum target, size_t baseIndex, std::vector<BufferBinding> &buffers);

    void activateSampledImages();
    void activateSamplers();

    std::mutex bindMutex;
    std::vector<BufferBinding> uniformBuffers;
    std::vector<BufferBinding> storageBuffers;
    std::vector<TextureBinding> sampledImages;
    std::vector<GLuint> samplers;
};

#endif //AGPU_GL_SHADER_RESOURCE_BINDING_HPP
