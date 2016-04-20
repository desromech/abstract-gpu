#ifndef AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
#define AGPU_METAL_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"
#include <vector>

struct UniformBufferBinding
{
    UniformBufferBinding()
        : buffer(0), offset(0), size(0) {}

    void reset();

    agpu_buffer *buffer;
    agpu_size offset;
    agpu_size size;
};

struct _agpu_shader_resource_binding : public Object<_agpu_shader_resource_binding>
{
public:
    _agpu_shader_resource_binding(agpu_device *device);
    void lostReferences();

    static agpu_shader_resource_binding *create(agpu_device *device, const ShaderSignatureElement &description);

    agpu_error bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer );
    agpu_error bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
    agpu_error bindTexture ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp );
    agpu_error bindTextureArrayRange ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp );
    agpu_error createSampler ( agpu_int location, agpu_sampler_description* description );

    agpu_error activateOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder);

    agpu_device *device;

    ShaderSignatureElement description;
    std::vector<UniformBufferBinding> uniformBindings;
    std::vector<agpu_texture*> textures;
    std::vector<id<MTLSamplerState> > samplerStates;

private:
    agpu_error activateUniformBindingsOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder);
    agpu_error activateSamplersOn(id<MTLRenderCommandEncoder> encoder);
    agpu_error activateTexturesOn(id<MTLRenderCommandEncoder> encoder);

};

#endif // AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
