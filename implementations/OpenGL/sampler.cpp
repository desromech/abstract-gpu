#include "sampler.hpp"
#include "constants.hpp"

namespace AgpuGL
{
inline GLenum mapMagFilter(agpu_filter filter)
{
    switch (filter)
    {
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:
        return GL_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:
        return GL_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:
        return GL_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:
    case AGPU_FILTER_ANISOTROPIC:
    default:
        return GL_LINEAR;
    }
}

inline GLenum mapMinFilter(agpu_filter filter)
{
    switch (filter)
    {
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:    return GL_NEAREST_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:     return GL_NEAREST_MIPMAP_LINEAR;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:     return GL_NEAREST_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:      return GL_NEAREST_MIPMAP_LINEAR;

    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:     return GL_LINEAR_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:      return GL_LINEAR_MIPMAP_LINEAR;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:      return GL_LINEAR_MIPMAP_NEAREST;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:       return GL_LINEAR_MIPMAP_LINEAR;
    case AGPU_FILTER_ANISOTROPIC:                               return GL_LINEAR_MIPMAP_LINEAR;
    default:
        return GL_NEAREST;
    }
}

inline GLenum mapAddressMode(agpu_texture_address_mode mode)
{
    switch (mode)
    {
    default:
    case AGPU_TEXTURE_ADDRESS_MODE_WRAP:    return GL_REPEAT;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR:  return GL_MIRRORED_REPEAT;
    case AGPU_TEXTURE_ADDRESS_MODE_CLAMP:   return GL_CLAMP_TO_EDGE;
    case AGPU_TEXTURE_ADDRESS_MODE_BORDER:  return GL_CLAMP;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR_ONCE: return GL_MIRROR_CLAMP_TO_EDGE;
    }
}

GLSampler::GLSampler(const agpu::device_ref &device)
    : device(device)
{
}

GLSampler::~GLSampler()
{
    deviceForGL->onMainContextBlocking([&]() {
        deviceForGL->glDeleteSamplers(1, &handle);
    });
}

agpu::sampler_ref GLSampler::create(const agpu::device_ref &device, agpu_sampler_description *description)
{
    if(!description)
        return agpu::sampler_ref();

    auto glDevice = device.as<GLDevice> ();
    GLuint handle;
    glDevice->onMainContextBlocking([&]{
        glDevice->glGenSamplers(1, &handle);
        glDevice->glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, mapMagFilter(description->filter));
        glDevice->glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, mapMinFilter(description->filter));
        glDevice->glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, mapAddressMode(description->address_u));
        glDevice->glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, mapAddressMode(description->address_v));
        glDevice->glSamplerParameterf(handle, GL_TEXTURE_MIN_LOD, description->min_lod);
        glDevice->glSamplerParameterf(handle, GL_TEXTURE_MAX_LOD, description->max_lod);
        glDevice->glSamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, description->comparison_enabled ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE);
        glDevice->glSamplerParameteri(handle, GL_TEXTURE_COMPARE_FUNC, mapCompareFunction(description->comparison_function));
    });

    auto result = agpu::makeObject<GLSampler> (device);
    auto sampler = result.as<GLSampler> ();
    sampler->description = *description;
    sampler->handle = handle;
    return result;
}

} // End of namespace AgpuGL
