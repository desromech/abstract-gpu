#ifndef AGPU_TEXTURE_HPP
#define AGPU_TEXTURE_HPP

#include "device.hpp"

struct _agpu_texture : public Object<_agpu_texture>
{
public:
    _agpu_texture();

    void lostReferences();

    static agpu_texture *create(agpu_device *device, agpu_texture_description *description, agpu_pointer initialData);

public:
    agpu_device *device;
    agpu_texture_description description;
    GLuint handle;
    GLenum target;

private:
    static void allocateTexture(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture1D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture2D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture3D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTextureCube(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTextureBuffer(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
};

#endif //AGPU_TEXTURE_HPP
