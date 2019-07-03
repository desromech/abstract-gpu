#include <algorithm>
#include <string.h>
#include "buffer.hpp"
#include "texture.hpp"
#include "texture_formats.hpp"
#include "texture_view.hpp"

namespace AgpuGL
{

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

        width = (agpu_size)std::max(compressedBlockWidth, (width + compressedBlockWidth - 1)/compressedBlockWidth*compressedBlockWidth);
        height = (agpu_size)std::max(compressedBlockHeight, (height + compressedBlockHeight - 1)/compressedBlockHeight*compressedBlockHeight);

        pitch = agpu_int(width / compressedBlockWidth * compressedBlockSize);
        slicePitch = agpu_int(pitch * (height / compressedBlockHeight));
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

void GLTexture::allocateTexture1D(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    if(description->layers > 1)
        deviceForGL->glTexStorage2D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->layers);
    else
        deviceForGL->glTexStorage1D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width);
}

void GLTexture::allocateTexture2D(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    if(description->sample_count > 1)
    {
        deviceForGL->glTexStorage2DMultisample(target, description->sample_count, mapInternalTextureFormat(description->format), description->width, description->height, GL_FALSE);
    }
    else
    {
        if(description->layers > 1)
            deviceForGL->glTexStorage3D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height, description->layers);
        else
            deviceForGL->glTexStorage2D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height);
    }
}

void GLTexture::allocateTexture3D(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    deviceForGL->glTexStorage3D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height, description->layers);
}

void GLTexture::allocateTextureCube(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    deviceForGL->glTexStorage2D(target, description->miplevels, mapInternalTextureFormat(description->format), description->width, description->height);
}

void GLTexture::allocateTextureBuffer(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description)
{
    glBindTexture(target, handle);
    // Do nothing here.
}

void GLTexture::allocateTexture(const agpu::device_ref &device, GLuint handle, GLenum target, agpu_texture_description *description)
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

GLTexture::GLTexture()
    : transferBuffer(0), mappedLevel(0), mappedPointer(nullptr)
{
}

GLTexture::~GLTexture()
{
    deviceForGL->onMainContextBlocking([&]() {
        if(transferBuffer)
        {
            deviceForGL->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
            deviceForGL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
            deviceForGL->glDeleteBuffers(1, &transferBuffer);
        }
        glDeleteTextures(1, &handle);
    });
}

agpu::texture_ref GLTexture::create(const agpu::device_ref &device, agpu_texture_description *description)
{
    GLuint handle;
    GLenum target = findTextureTarget(description);

    deviceForGL->onMainContextBlocking([&]() {
        glGenTextures(1, &handle);
        allocateTexture(device, handle, target, description);
        glFinish();
    });

    auto result = agpu::makeObject<GLTexture> ();
    auto texture = result.as<GLTexture> ();
    texture->device = device;
    texture->handle = handle;
    texture->target = target;
    texture->description = *description;
    texture->isCompressed = isCompressedTextureFormat(description->format);
    return result;
}


void GLTexture::createTransferBuffer(GLenum target)
{
    auto bufferSize = BufferTextureTransferLayout::fromDescriptionAndLevel(description, 0).size;
    deviceForGL->glGenBuffers(1, &transferBuffer);
    deviceForGL->glBindBuffer(target, transferBuffer);
    deviceForGL->glBufferStorage(target, bufferSize, nullptr, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT |  GL_CLIENT_STORAGE_BIT);
}

void GLTexture::performTransferToCpu(int level)
{
    deviceForGL->glBindBuffer(GL_PIXEL_PACK_BUFFER, transferBuffer);
    glBindTexture(target, handle);
    bool isArray = description.layers > 1;
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

    deviceForGL->glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
}

void GLTexture::performTransferToGpu(int level, int arrayIndex)
{
    deviceForGL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, transferBuffer);
    glBindTexture(target, handle);
    auto transferLayout = BufferTextureTransferLayout::fromDescriptionAndLevel(description, level);
    bool isArray = description.layers > 1;
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
                deviceForGL->glCompressedTexSubImage3D(target, level, 0, 0, arrayIndex, width, height, 1, mapInternalTextureFormat(description.format), transferLayout.size, 0);
            else
                deviceForGL->glCompressedTexSubImage2D(target, level, 0, 0, width, height, mapInternalTextureFormat(description.format), transferLayout.size, 0);
        }
        else
        {
            if(isArray)
                deviceForGL->glTexSubImage3D(target, level, 0, 0, arrayIndex, width, height, 1, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
            else
                glTexSubImage2D(target, level, 0, 0, width, height, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
        }
        break;
    case AGPU_TEXTURE_3D:
        if(isCompressed)
        {
            deviceForGL->glCompressedTexSubImage3D(target, level, 0, 0, 0, width, height, depth, mapInternalTextureFormat(description.format), transferLayout.size, 0);
        }
        else
        {
            deviceForGL->glTexSubImage3D(target, level, 0, 0, 0, width, height, depth, mapExternalFormat(description.format), mapExternalFormatType(description.format), 0);
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
                deviceForGL->glCompressedTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + arrayIndex, level, 0, 0, width, height, mapInternalTextureFormat(description.format), transferLayout.size, 0);
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
    deviceForGL->glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

agpu_pointer GLTexture::mapLevel ( agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d *region )
{
    if(mappedPointer)
        return mappedPointer;

    this->mappingAccess = flags;
    this->mappedLevel = level;
    bool isRead = mappingAccess == AGPU_READ_ONLY || mappingAccess == AGPU_READ_WRITE;
    bool isWrite = mappingAccess == AGPU_WRITE_ONLY || mappingAccess == AGPU_READ_WRITE;

    GLenum target = isWrite ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER;
    GLenum access = mapMappingAccess(mappingAccess);

    deviceForGL->onMainContextBlocking([&]() {
        if(!transferBuffer)
            createTransferBuffer(GL_PIXEL_PACK_BUFFER);

        if(isRead)
            performTransferToCpu(level);

        deviceForGL->glBindBuffer(target, transferBuffer);
        mappedPointer = deviceForGL->glMapBuffer(target, access);
        deviceForGL->glBindBuffer(target, 0);
    });

    mappedArrayIndex = arrayIndex;

    return mappedPointer;
}

agpu_error GLTexture::unmapLevel ( )
{
    if(!mappedPointer)
        return AGPU_INVALID_OPERATION;

    //bool isRead = mappingAccess == AGPU_READ_ONLY || mappingAccess == AGPU_READ_WRITE;
    bool isWrite = mappingAccess == AGPU_WRITE_ONLY || mappingAccess == AGPU_READ_WRITE;
    GLenum target = isWrite ? GL_PIXEL_UNPACK_BUFFER : GL_PIXEL_PACK_BUFFER;

    deviceForGL->onMainContextBlocking([&]() {
        deviceForGL->glBindBuffer(target, transferBuffer);
        deviceForGL->glUnmapBuffer(target);
        deviceForGL->glBindBuffer(target, 0);

        if(isWrite)
            performTransferToGpu(mappedLevel, mappedArrayIndex);
    });

    mappedPointer = nullptr;
    return AGPU_OK;
}

agpu_error GLTexture::readTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int dstPitch, agpu_int slicePitch, agpu_pointer data )
{
    CHECK_POINTER(data);
    auto src = reinterpret_cast<uint8_t*> (mapLevel(level, arrayIndex, AGPU_READ_ONLY, nullptr));
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

agpu_error GLTexture::uploadTextureData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    return uploadTextureSubData(level, arrayIndex, pitch, slicePitch, nullptr, nullptr, data);
}

agpu_error GLTexture::uploadTextureSubData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data )
{
    auto dst = reinterpret_cast<uint8_t*> (mapLevel(level, arrayIndex, AGPU_WRITE_ONLY, destRegion));
    if(!dst)
        return AGPU_ERROR;

    auto transferLayout = BufferTextureTransferLayout::fromDescriptionAndLevel(description, level);
    auto dstPitch = transferLayout.pitch;
    auto src = reinterpret_cast<uint8_t*> (data);

    // Copy the 2D texture slice.
    if (pitch >= transferLayout.pitch && slicePitch == transferLayout.slicePitch && !sourceSize && !destRegion)
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

agpu_error GLTexture::discardUploadBuffer()
{
    return AGPU_OK;
}

agpu_error GLTexture::discardReadbackBuffer()
{
    return AGPU_OK;
}

agpu_error GLTexture::getFullViewDescription(agpu_texture_view_description *viewDescription)
{
    CHECK_POINTER(viewDescription);
    memset(viewDescription, 0, sizeof(*viewDescription));
    viewDescription->type = description.type;
    viewDescription->format = description.format;
    viewDescription->components.r = AGPU_COMPONENT_SWIZZLE_R;
    viewDescription->components.g = AGPU_COMPONENT_SWIZZLE_G;
    viewDescription->components.b = AGPU_COMPONENT_SWIZZLE_B;
    viewDescription->components.a = AGPU_COMPONENT_SWIZZLE_A;
    viewDescription->subresource_range.usage_mode = description.usage_modes;
    viewDescription->subresource_range.base_miplevel = 0;
    viewDescription->subresource_range.level_count = description.miplevels;
    viewDescription->subresource_range.base_arraylayer = 0;
    viewDescription->subresource_range.layer_count = description.layers;
    return AGPU_OK;
}


agpu::texture_view_ptr GLTexture::createView(agpu_texture_view_description* description)
{
    return nullptr;
}

agpu::texture_view_ptr GLTexture::getOrCreateFullView()
{
    if(!fullTextureView)
    {
        agpu_texture_view_description descripton;
        getFullViewDescription(&descripton);
        fullTextureView = agpu::makeObject<GLFullTextureView> (device, refFromThis<agpu::texture> (), descripton);
    }

    return fullTextureView.disownedNewRef();
}

agpu_error GLTexture::getDescription(agpu_texture_description* description)
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

} // End of namespace AgpuGL
