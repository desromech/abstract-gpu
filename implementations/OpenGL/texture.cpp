#include <string.h>
#include "buffer.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"

void _agpu_texture::allocateTexture1D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    if(description->depthOrArraySize > 1)
        device->glTexStorage2D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->depthOrArraySize);
    else
        device->glTexStorage1D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width);
}

void _agpu_texture::allocateTexture2D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    if(description->depthOrArraySize > 1)
        device->glTexStorage3D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height, description->depthOrArraySize);
    else
        device->glTexStorage2D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height);
}

void _agpu_texture::allocateTexture3D(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    device->glTexStorage3D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height, description->depthOrArraySize);
}

void _agpu_texture::allocateTextureCube(agpu_device *device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    device->glTexStorage2D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height);
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
    : transferBuffer(0), mappedLevel(0), mappedPointer(nullptr)
{
}

void _agpu_texture::lostReferences()
{
    device->onMainContextBlocking([&]() {
        if(transferBuffer)
        {
            device->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            device->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            device->glDeleteBuffers(1, &transferBuffer);
        }
        glDeleteTextures(1, &handle);
    });
}

agpu_texture *_agpu_texture::create(agpu_device *device, agpu_texture_description *description)
{
    GLuint handle;
    GLenum target = findTextureTarget(description);

    device->onMainContextBlocking([&]() {
        glGenTextures(1, &handle);
        allocateTexture(device, handle, target, description);
        glFinish();
    });

    auto texture = new agpu_texture();
    texture->device = device;
    texture->handle = handle;
    texture->target = target;
    texture->description = *description;
    return texture;
}

size_t _agpu_texture::getPixelSize()
{
    return pixelSizeOfTextureFormat(description.format);
}

size_t _agpu_texture::pitchOfLevel(int level)
{
    size_t pixelSize = getPixelSize();
    auto width = description.width >> level;
    return (pixelSize * width + 3) & (~3);
}

size_t _agpu_texture::sizeOfLevel(int level)
{
    auto pitch = pitchOfLevel(level);
    int height = (description.height >> level);
    if (!height)
        height = 1;
    int depth = (description.height >> level);
    if (!depth)
        depth = 1;

    switch(description.type)
    {
    case AGPU_TEXTURE_1D:
        return pitch * description.depthOrArraySize;
    case AGPU_TEXTURE_BUFFER:
        return pitch * description.depthOrArraySize;
    case AGPU_TEXTURE_2D:
        return pitch * height * description.depthOrArraySize;
    case AGPU_TEXTURE_3D:
        return pitch * height * (description.depthOrArraySize >> level);
    case AGPU_TEXTURE_CUBE:
        return pitch * height * description.depthOrArraySize;
    default:
        abort();
    }
}

void _agpu_texture::createTransferBuffer(GLenum target)
{
    auto bufferSize = sizeOfLevel(0);
    device->glGenBuffers(1, &transferBuffer);
    device->glBindBuffer(target, transferBuffer);
    device->glBufferStorage(target, bufferSize, nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT |  GL_CLIENT_STORAGE_BIT);
}

void _agpu_texture::performTransferToCpu(int level)
{
    device->glBindBuffer(GL_PIXEL_PACK_BUFFER, transferBuffer);
    glBindTexture(target, handle);
    bool isArray = description.depthOrArraySize > 1;
    if(isArray)
        return; // Can't support it.

    glGetTexImage(target, level, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
    glFinish();
    
    device->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void _agpu_texture::performTransferToGpu(int level, int arrayIndex)
{
    device->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, transferBuffer);
    glBindTexture(target, handle);
    bool isArray = description.depthOrArraySize > 1;
    int width = description.width >> level;
    if (!width)
        width = 1;
    int height = description.height >> level;
    if (!height)
        height = 1;
    int depth = description.depthOrArraySize >> level;
    if (!depth)
        depth = 1;
    int arraySize = description.depthOrArraySize;

    switch(description.type)
    {
    case AGPU_TEXTURE_1D:
        if(isArray)
            glTexSubImage2D(target, level, 0, arrayIndex, width, 1, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        else
            glTexSubImage1D(target, level, 0, width, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        break;
    case AGPU_TEXTURE_BUFFER:
        break;
    case AGPU_TEXTURE_2D:
        if(isArray)
            device->glTexSubImage3D(target, level, 0, 0, arrayIndex, width, height, 1, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        else
            glTexSubImage2D(target, level, 0, 0, width, height, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        break;
    case AGPU_TEXTURE_3D:
        device->glTexSubImage3D(target, level, 0, 0, 0, width, height, depth, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        break;
    case AGPU_TEXTURE_CUBE:
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + arrayIndex, level, 0, 0, width, height, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        break;
    default:
        abort();
    }

    glFinish();
    device->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

agpu_pointer _agpu_texture::mapLevel ( agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags )
{
    if(mappedPointer)
        return mappedPointer;

    this->mappingAccess = flags;
    this->mappedLevel = level;
    bool isRead = mappingAccess == AGPU_READ_ONLY || mappingAccess == AGPU_READ_WRITE;
    bool isWrite = mappingAccess == AGPU_WRITE_ONLY || mappingAccess == AGPU_READ_WRITE;

    GLenum target = isWrite ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER;
    GLenum access = mapMappingAccess(mappingAccess);

    device->onMainContextBlocking([&]() {
        if(!transferBuffer)
            createTransferBuffer(GL_PIXEL_PACK_BUFFER);

        if(isRead)
            performTransferToCpu(level);

        device->glBindBuffer(target, transferBuffer);
        mappedPointer = device->glMapBuffer(target, access);
        device->glBindBuffer(target, 0);
    });

    mappedArrayIndex = arrayIndex;

    return mappedPointer;
}

agpu_error _agpu_texture::unmapLevel ( )
{
    if(!mappedPointer)
        return AGPU_INVALID_OPERATION;

    //bool isRead = mappingAccess == AGPU_READ_ONLY || mappingAccess == AGPU_READ_WRITE;
    bool isWrite = mappingAccess == AGPU_WRITE_ONLY || mappingAccess == AGPU_READ_WRITE;
    GLenum target = isWrite ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER;

    device->onMainContextBlocking([&]() {
        device->glBindBuffer(target, transferBuffer);
        device->glUnmapBuffer(target);
        device->glBindBuffer(target, 0);

        if(isWrite)
            performTransferToGpu(mappedLevel, mappedArrayIndex);
    });

    mappedPointer = nullptr;
    return AGPU_OK;
}

agpu_error _agpu_texture::readTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int dstPitch, agpu_int slicePitch, agpu_pointer data )
{
    CHECK_POINTER(data);
    auto src = reinterpret_cast<uint8_t*> (mapLevel(level, arrayIndex, AGPU_READ_ONLY));
    if(!src)
        return AGPU_ERROR;

    auto srcPitch = pitchOfLevel(level);
    auto dst = reinterpret_cast<uint8_t*> (data);
    if(description.type == AGPU_TEXTURE_1D)
    {
        memcpy(dst, src, srcPitch);
        unmapLevel();
        return AGPU_OK;
    }
    else if(description.type == AGPU_TEXTURE_3D)
    {
        // TODO: Implement this.
        unmapLevel();
        return AGPU_OK;
    }

    auto height = description.height >> level;
    auto fsrc = src + (height - 1) *srcPitch;
    ptrdiff_t fsrcPitch = -ptrdiff_t(srcPitch);
    for(size_t y = 0; y < height; ++y)
    {
        memcpy(dst + dstPitch*y, fsrc + fsrcPitch*y, srcPitch);
    }

    unmapLevel();
    return AGPU_OK;
}

agpu_error _agpu_texture::uploadTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    auto dst = reinterpret_cast<uint8_t*> (mapLevel(level, arrayIndex, AGPU_WRITE_ONLY));
    if(!dst)
        return AGPU_ERROR;

    auto srcPitchAbs = pitch;
    if(srcPitchAbs < 0)
        srcPitchAbs = -srcPitchAbs;

    auto dstPitch = pitchOfLevel(level);
    auto src = reinterpret_cast<uint8_t*> (data);
    if(description.type == AGPU_TEXTURE_1D)
    {
        memcpy(dst, src, srcPitchAbs);
        unmapLevel();
        return AGPU_OK;
    }
    else if(description.type == AGPU_TEXTURE_3D)
    {
        // TODO: Implement this.
        unmapLevel();
        return AGPU_OK;
    }

    auto height = description.height >> level;
    auto fdst = dst + (height - 1)*dstPitch;
    ptrdiff_t fdstPitch = -ptrdiff_t(dstPitch);
    for(size_t y = 0; y < height; ++y)
    {
        memcpy(fdst + fdstPitch*y, src + pitch*y, srcPitchAbs);
    }

    unmapLevel();
    return AGPU_OK;
}

agpu_error _agpu_texture::discardUploadBuffer()
{
    return AGPU_OK;
}

agpu_error _agpu_texture::discardReadbackBuffer()
{
    return AGPU_OK;

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

AGPU_EXPORT agpu_error agpuGetTextureDescription ( agpu_texture* texture, agpu_texture_description* description )
{
    CHECK_POINTER(texture);
    CHECK_POINTER(description);

    *description = texture->description;
    return AGPU_OK;
}

AGPU_EXPORT agpu_pointer agpuMapTextureLevel ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags )
{
    if(!texture)
        return nullptr;

    return texture->mapLevel(level, arrayIndex, flags);
}

AGPU_EXPORT agpu_error agpuUnmapTextureLevel ( agpu_texture* texture )
{
    CHECK_POINTER(texture);
    return texture->unmapLevel();
}

AGPU_EXPORT agpu_error agpuReadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    CHECK_POINTER(texture);
    return texture->readTextureData(level, arrayIndex, pitch, slicePitch, data);
}

AGPU_EXPORT agpu_error agpuUploadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    CHECK_POINTER(texture);
    return texture->uploadTextureData(level, arrayIndex, pitch, slicePitch, data);
}

AGPU_EXPORT agpu_error agpuDiscardTextureUploadBuffer(agpu_texture* texture)
{
    CHECK_POINTER(texture);
    return texture->discardUploadBuffer();
}

AGPU_EXPORT agpu_error agpuDiscardTextureReadbackBuffer(agpu_texture* texture)
{
    CHECK_POINTER(texture);
    return texture->discardReadbackBuffer();
}
