#ifndef AGPU_GL_SHADER_RESOURCE_BINDING_HPP
#define AGPU_GL_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"

struct UniformBinding
{
    UniformBinding() : buffer(nullptr), range(false), offset(0), size(-1) {}

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

    agpu_error bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer);
    agpu_error bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size);

    agpu_error bindTexture(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodClamp);
    agpu_error bindTextureArrayRange(agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodClamp);

    agpu_error createSampler(agpu_int location, agpu_sampler_description* description);

public:
    void activate();

    agpu_device *device;
    agpu_shader_signature *signature;
    int elementIndex;

private:

    std::mutex bindMutex;
    std::vector<UniformBinding> uniformBuffers;
    std::vector<TextureBinding> textures;
    std::vector<GLuint> samplers;
    agpu_shader_binding_type type;
    int startIndex;
};

#endif //AGPU_GL_SHADER_RESOURCE_BINDING_HPP
