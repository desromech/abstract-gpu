#include <algorithm>
#include <string.h>
#include "buffer.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"

BufferTextureTransferLayout BufferTextureTransferLayout::fromDescriptionAndLevel(const agpu_texture_description &description, int level)
{
    BufferTextureTransferLayout result;
    result.setFromDescriptionAndLevel(description, level);
    return result;
}

void BufferTextureTransferLayout::setFromDescriptionAndLevel(const agpu_texture_description &description, int level)
{
    height = std::max(1u, description.height >> level);
    width = std::max(1u, description.width >> level);
    depthOrArraySize = std::max(1u, depthOrArraySize >> level);
    if(description.type != AGPU_TEXTURE_3D || depthOrArraySize == 0)
        depthOrArraySize = 1;

    logicalWidth = width;
    logicalHeight = height;
    logicalDepthOrArraySize = depthOrArraySize;

    if(isCompressedTextureFormat(description.format))
    {
        auto compressedBlockSize = blockSizeOfCompressedTextureFormat(description.format);
        auto compressedBlockWidth = blockWidthOfCompressedTextureFormat(description.format);
        auto compressedBlockHeight = blockHeightOfCompressedTextureFormat(description.format);

        width = std::max(compressedBlockWidth, (width + compressedBlockWidth - 1)/compressedBlockWidth*compressedBlockWidth);
        height = std::max(compressedBlockHeight, (height + compressedBlockHeight - 1)/compressedBlockHeight*compressedBlockHeight);

        pitch = width / compressedBlockWidth * compressedBlockSize;
        slicePitch = pitch * (height / compressedBlockHeight);
        size = slicePitch*depthOrArraySize;
    }
    else
    {
        auto uncompressedPixelSize = pixelSizeOfTextureFormat(description.format);
        pitch = (width*uncompressedPixelSize + 3) & -4;
        slicePitch = height * pitch;
        size = slicePitch*depthOrArraySize;
    }
}

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
    texture->isCompressed = isCompressedTextureFormat(description->format);
    return texture;
}


void _agpu_texture::createTransferBuffer(GLenum target)
{
    auto bufferSize = BufferTextureTransferLayout::fromDescriptionAndLevel(description, 0).size;
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

    if(isCompressed)
    {
        printf("TODO: Readback compressed texture\n");
    }
    else
    {
        glGetTexImage(target, level, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
    }
    glFinish();

    device->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void _agpu_texture::performTransferToGpu(int level, int arrayIndex)
{
    device->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, transferBuffer);
    glBindTexture(target, handle);
    auto transferLayout = BufferTextureTransferLayout::fromDescriptionAndLevel(description, level);
    bool isArray = description.depthOrArraySize > 1;
    auto width = transferLayout.logicalWidth;
    auto height = transferLayout.logicalHeight;
    auto depth = transferLayout.logicalDepthOrArraySize;

    switch(description.type)
    {
    case AGPU_TEXTURE_1D:
        if(isCompressed)
        {

        }
        else
        {
            if(isArray)
                glTexSubImage2D(target, level, 0, arrayIndex, width, 1, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
            else
                glTexSubImage1D(target, level, 0, width, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        }
        break;
    case AGPU_TEXTURE_BUFFER:
        break;
    case AGPU_TEXTURE_2D:
        if(isCompressed)
        {
            if(isArray)
                device->glCompressedTexSubImage3D(target, level, 0, 0, arrayIndex, width, height, 1, mapInternalTextureFormat(description.format), transferLayout.size, 0);
            else
                device->glCompressedTexSubImage2D(target, level, 0, 0, width, height, mapInternalTextureFormat(description.format), transferLayout.size, 0);
        }
        else
        {
            if(isArray)
                device->glTexSubImage3D(target, level, 0, 0, arrayIndex, width, height, 1, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
            else
                glTexSubImage2D(target, level, 0, 0, width, height, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        }
        break;
    case AGPU_TEXTURE_3D:
        if(isCompressed)
        {
            device->glCompressedTexSubImage3D(target, level, 0, 0, 0, width, height, depth, mapInternalTextureFormat(description.format), transferLayout.size, 0);
        }
        else
        {
            device->glTexSubImage3D(target, level, 0, 0, 0, width, height, depth, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        }
        break;
    case AGPU_TEXTURE_CUBE:
        if(isCompressed)
        {
            if(isArray)
            {
                // TODO: Implement this case
            }
            else
            {
                device->glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + arrayIndex, level, 0, 0, width, height, mapInternalTextureFormat(description.format), transferLayout.size, 0);
            }
        }
        else
        {
            if(isArray)
            {
                // TODO: Implement this case
            }
            else
            {
                glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + arrayIndex, level, 0, 0, width, height, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
            }
        }
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

    auto transferLayout = BufferTextureTransferLayout::fromDescriptionAndLevel(description, level);
    auto srcPitch = transferLayout.pitch;
    auto dst = reinterpret_cast<uint8_t*> (data);

    if (dstPitch >= transferLayout.pitch && slicePitch == transferLayout.slicePitch)
    {
        memcpy(dst, src, slicePitch);
    }
    else
    {
        auto height = transferLayout.height;
        auto fsrc = src + (height - 1) *srcPitch;
        ptrdiff_t fsrcPitch = -ptrdiff_t(srcPitch);
        for(size_t y = 0; y < height; ++y)
        {
            memcpy(dst + dstPitch*y, fsrc + fsrcPitch*y, srcPitch);
        }
    }

    unmapLevel();
    return AGPU_OK;
}

agpu_error _agpu_texture::uploadTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    auto dst = reinterpret_cast<uint8_t*> (mapLevel(level, arrayIndex, AGPU_WRITE_ONLY));
    if(!dst)
        return AGPU_ERROR;

    auto transferLayout = BufferTextureTransferLayout::fromDescriptionAndLevel(description, level);
    auto dstPitch = transferLayout.pitch;
    auto src = reinterpret_cast<uint8_t*> (data);

    // Copy the 2D texture slice.
    if (pitch >= transferLayout.pitch && slicePitch == transferLayout.slicePitch)
    {
        memcpy(dst, src, slicePitch);
    }
    else
    {
        auto srcPitchAbs = pitch;
        if(srcPitchAbs < 0)
            srcPitchAbs = -srcPitchAbs;

        auto height = transferLayout.height;
        auto fdst = dst + (height - 1)*dstPitch;
        ptrdiff_t fdstPitch = -ptrdiff_t(dstPitch);
        for (size_t y = 0; y < height; ++y)
        {
            memcpy(fdst, src, srcPitchAbs);
            fdst += fdstPitch;
            src += pitch;
        }
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

agpu_error _agpu_texture::getFullViewDescription(agpu_texture_view_description *viewDescription)
{
    CHECK_POINTER(viewDescription);
    memset(viewDescription, 0, sizeof(*viewDescription));
    viewDescription->type = description.type;
    viewDescription->texture = this;
    viewDescription->format = description.format;
    viewDescription->components.r = AGPU_COMPONENT_SWIZZLE_R;
    viewDescription->components.g = AGPU_COMPONENT_SWIZZLE_G;
    viewDescription->components.b = AGPU_COMPONENT_SWIZZLE_B;
    viewDescription->components.a = AGPU_COMPONENT_SWIZZLE_A;
    viewDescription->subresource_range.usage_flags = description.flags;
    viewDescription->subresource_range.base_miplevel = 0;
    viewDescription->subresource_range.level_count = description.miplevels;
    viewDescription->subresource_range.base_arraylayer = 0;
    viewDescription->subresource_range.layer_count = description.depthOrArraySize;
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

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription(agpu_texture* texture, agpu_texture_view_description *description)
{
    CHECK_POINTER(texture);
    return texture->getFullViewDescription(description);
}
