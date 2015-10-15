#ifndef AGPU_TEXTURE_HPP
#define AGPU_TEXTURE_HPP

#include "device.hpp"

struct _agpu_texture : public Object<_agpu_texture>
{
public:
    _agpu_texture();

    void lostReferences();

    static agpu_texture *create(agpu_device *device, agpu_texture_description *description);

    agpu_pointer mapLevel ( agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags );
    agpu_error unmapLevel ( );

    agpu_error readTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data );
    agpu_error uploadTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data );

    agpu_error discardUploadBuffer();
    agpu_error discardReadbackBuffer();

public:
    agpu_device *device;
    agpu_texture_description description;
    GLuint handle;
    GLenum target;

    GLuint transferBuffer;
    agpu_mapping_access mappingAccess;
    agpu_int mappedLevel;
    agpu_uint mappedArrayIndex;
    agpu_pointer mappedPointer;

private:
    static void allocateTexture(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture1D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture2D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTexture3D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTextureCube(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);
    static void allocateTextureBuffer(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description);

    size_t getPixelSize();
    size_t pitchOfLevel(int level);
    size_t sizeOfLevel(int level);

    void createTransferBuffer(GLenum target);
    void performTransferToCpu(int level);
    void performTransferToGpu(int level, int arrayIndex);
};

#endif //AGPU_TEXTURE_HPP
