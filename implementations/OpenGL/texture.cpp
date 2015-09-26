#include "texture.hpp"
#include "texture_formats.hpp"

void _agpu_texture::allocateTexture1D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    if(description->depthOrArraySize)
        device->glTexStorage2D(target, description->miplevels, mapTextureFormat(description->format), description->width, description->depthOrArraySize);
    else
        device->glTexStorage1D(target, description->miplevels, mapTextureFormat(description->format), description->width);
}

void _agpu_texture::allocateTexture2D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    if(description->depthOrArraySize)
        device->glTexStorage3D(target, description->miplevels, mapTextureFormat(description->format), description->width, description->height, description->depthOrArraySize);
    else
        device->glTexStorage2D(target, description->miplevels, mapTextureFormat(description->format), description->width, description->height);
}

void _agpu_texture::allocateTexture3D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    device->glTexStorage3D(target, description->miplevels, mapTextureFormat(description->format), description->width, description->height, description->depthOrArraySize);
}

void _agpu_texture::allocateTextureCube(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    device->glTexStorage2D(target, description->miplevels, mapTextureFormat(description->format), description->width, description->height);
}

void _agpu_texture::allocateTextureBuffer(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    // Do nothing here.
}

void _agpu_texture::allocateTexture(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    switch(description->type)
    {
    case AGPU_TEXTURE_1D:
        return allocateTexture1D(device, handle, target, description);
    case AGPU_TEXTURE_2D:
        return allocateTexture2D(device, handle, target, description);
    case AGPU_TEXTURE_3D:
        return allocateTexture3D(device, handle, target, description);
    case AGPU_TEXTURE_CUBE:
        return allocateTextureCube(device, handle, target, description);
    case AGPU_TEXTURE_BUFFER:
        return allocateTextureBuffer(device, handle, target, description);
    default:
        abort();
    }
}

_agpu_texture::_agpu_texture()
{
}

void _agpu_texture::lostReferences()
{
    device->onMainContextBlocking([&]() {
        glDeleteTextures(1, &handle);
    });
}

agpu_texture *_agpu_texture::create(agpu_device *device, agpu_texture_description *description, agpu_pointer initialData)
{
    GLuint handle;
    GLenum target = findTextureTarget(description);

    device->onMainContextBlocking([&]() {
        glGenTextures(1, &handle);
        allocateTexture(device, handle, target, description);
    });

    auto texture = new agpu_texture();
    texture->device = device;
    texture->handle = handle;
    texture->target = target;
    texture->description = *description;
    return texture;
}

// Exported C interface.
AGPU_EXPORT agpu_error agpuAddTextureReference ( agpu_texture* texture )
{
    CHECK_POINTER(texture);
    return texture->retain();
}

AGPU_EXPORT agpu_error agpuReleaseTexture ( agpu_texture* texture )
{
    CHECK_POINTER(texture);
    return texture->release();
}
