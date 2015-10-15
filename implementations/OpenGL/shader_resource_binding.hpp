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

struct _agpu_shader_resource_binding : public Object<_agpu_shader_resource_binding>
{
public:
    _agpu_shader_resource_binding();

    void lostReferences();

    static agpu_shader_resource_binding *create(agpu_shader_signature *signature, int elementIndex);

    agpu_error bindUniformBuffer(agpu_int location, agpu_buffer* uniform_buffer);
    agpu_error bindUniformBufferRange(agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size);

public:
    void activate();
    
    agpu_device *device;
    agpu_shader_signature *signature;
    int elementIndex;

private:
    std::vector<UniformBinding> uniformBuffers;
    agpu_shader_binding_type type;
    int startIndex;
};

#endif //AGPU_GL_SHADER_RESOURCE_BINDING_HPP
