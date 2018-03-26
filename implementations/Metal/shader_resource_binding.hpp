#ifndef AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
#define AGPU_METAL_SHADER_RESOURCE_BINDING_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"
#include <vector>

struct BufferBinding
{
    BufferBinding()
        : buffer(0), offset(0), size(0) {}
    ~BufferBinding();

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

    static agpu_shader_resource_binding *create(agpu_device *device, agpu_shader_signature *signature, agpu_uint elementIndex);

    agpu_error bindUniformBuffer ( agpu_int location, agpu_buffer* uniform_buffer );
    agpu_error bindUniformBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
    agpu_error bindStorageBuffer ( agpu_int location, agpu_buffer* uniform_buffer );
    agpu_error bindStorageBufferRange ( agpu_int location, agpu_buffer* uniform_buffer, agpu_size offset, agpu_size size );
    agpu_error bindTexture ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_float lodclamp );
    agpu_error bindTextureArrayRange ( agpu_int location, agpu_texture* texture, agpu_uint startMiplevel, agpu_int miplevels, agpu_int firstElement, agpu_int numberOfElements, agpu_float lodclamp );
    agpu_error createSampler ( agpu_int location, agpu_sampler_description* description );

    agpu_error activateOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder);
    agpu_error activateComputeOn(id<MTLComputeCommandEncoder> encoder);

    agpu_device *device;
    agpu_shader_signature *signature;
    agpu_uint elementIndex;

    std::vector<BufferBinding> buffers;
    std::vector<agpu_texture*> textures;
    std::vector<id<MTLSamplerState> > samplers;

private:
    agpu_error activateBuffersOn(agpu_uint vertexBufferCount, id<MTLRenderCommandEncoder> encoder);
    agpu_error activateSamplersOn(id<MTLRenderCommandEncoder> encoder);
    agpu_error activateTexturesOn(id<MTLRenderCommandEncoder> encoder);

    agpu_error activateComputeBuffersOn(id<MTLComputeCommandEncoder> encoder);
    agpu_error activateComputeSamplersOn(id<MTLComputeCommandEncoder> encoder);
    agpu_error activateComputeTexturesOn(id<MTLComputeCommandEncoder> encoder);

};

#endif // AGPU_METAL_SHADER_RESOURCE_BINDING_HPP
