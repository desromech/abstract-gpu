#include "texture.hpp"

_agpu_texture::_agpu_texture(agpu_device *device)
    : device(device)
{
    handle = nil;
}

void _agpu_texture::lostReferences()
{
}

agpu_error _agpu_texture::getDescription ( agpu_texture_description* description )
{
    CHECK_POINTER(description);
    *description = this->description;
    return AGPU_OK;
}

agpu_pointer _agpu_texture::mapLevel ( agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags )
{
    // TODO: Implement this, if possible
    return nullptr;
}

agpu_error _agpu_texture::unmapLevel (  )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::readData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::uploadData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::discardUploadBuffer (  )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::discardReadbackBuffer (  )
{
    return AGPU_UNIMPLEMENTED;
}

agpu_error _agpu_texture::getFullViewDescription ( agpu_texture_view_description* result )
{
    return AGPU_UNIMPLEMENTED;
}

// The exported C interface
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
    return texture->getDescription(description);
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

AGPU_EXPORT agpu_error agpuReadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer )
{
    CHECK_POINTER(texture);
    return texture->readData(level, arrayIndex, pitch, slicePitch, buffer);
}

AGPU_EXPORT agpu_error agpuUploadTextureData ( agpu_texture* texture, agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data )
{
    CHECK_POINTER(texture);
    return texture->uploadData(level, arrayIndex, pitch, slicePitch, data);
}

AGPU_EXPORT agpu_error agpuDiscardTextureUploadBuffer ( agpu_texture* texture )
{
    CHECK_POINTER(texture);
    return texture->discardUploadBuffer();
}

AGPU_EXPORT agpu_error agpuDiscardTextureReadbackBuffer ( agpu_texture* texture )
{
    CHECK_POINTER(texture);
    return texture->discardReadbackBuffer();
}

AGPU_EXPORT agpu_error agpuGetTextureFullViewDescription ( agpu_texture* texture, agpu_texture_view_description* result )
{
    CHECK_POINTER(texture);
    return texture->getFullViewDescription(result);
}
