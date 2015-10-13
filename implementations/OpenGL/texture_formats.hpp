#ifndef AGPU_TEXTURE_FORMAT_HPP
#define AGPU_TEXTURE_FORMAT_HPP

#include <AGPU/agpu.h>

inline GLenum findTextureTarget(agpu_texture_description *description)
{
    assert(description);
    switch(description->type)
    {
    case AGPU_TEXTURE_1D:
        if(description->depthOrArraySize > 1)
            return GL_TEXTURE_1D_ARRAY;
        else
            return GL_TEXTURE_1D;
    case AGPU_TEXTURE_2D:
        if(description->depthOrArraySize > 1)
            return GL_TEXTURE_2D_ARRAY;
        else
            return GL_TEXTURE_2D;
    case AGPU_TEXTURE_3D:
        return GL_TEXTURE_3D;
    case AGPU_TEXTURE_CUBE:
        return GL_TEXTURE_CUBE_MAP;
    case AGPU_TEXTURE_BUFFER:
        return GL_TEXTURE_BUFFER;
    default:
        abort();

    }
}

inline GLenum mapInternalTextureFormat(agpu_texture_format format)
{
    switch(format)
    {
    case AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM: return GL_RGBA8;
    case AGPU_TEXTURE_FORMAT_D16_UNORM: return GL_DEPTH_COMPONENT16;
    default:
        abort();
    }
}

inline GLenum mapExternalFormat(agpu_texture_format format)
{
    switch(format)
    {
    case AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM: return GL_RGBA;
    case AGPU_TEXTURE_FORMAT_D16_UNORM: return GL_RED;
    default:
        abort();
    }
}

inline GLenum mapExternalFormatType(agpu_texture_format format)
{
    switch(format)
    {
    case AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM: return GL_UNSIGNED_BYTE;
    case AGPU_TEXTURE_FORMAT_D16_UNORM: return GL_UNSIGNED_SHORT;
    default:
        abort();
    }
}

inline size_t pixelSizeOfTextureFormat(agpu_texture_format format)
{
    switch(format)
    {
    case AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM: return 4;
    case AGPU_TEXTURE_FORMAT_D16_UNORM: return 2;
    default:
        abort();
    }
}

inline bool hasDepthComponent(agpu_texture_format format)
{
    switch(format)
    {
    case AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT:
    case AGPU_TEXTURE_FORMAT_D32_FLOAT:
    case AGPU_TEXTURE_FORMAT_D24_UNORM_S8_UINT:
    case AGPU_TEXTURE_FORMAT_D16_UNORM:
        return true;
    default:
        return false;
    }
}

inline bool hasStencilComponent(agpu_texture_format format)
{
    switch(format)
    {
    case AGPU_TEXTURE_FORMAT_D32_FLOAT_S8X24_UINT:
    case AGPU_TEXTURE_FORMAT_D24_UNORM_S8_UINT:
        return true;
    default:
        return false;
    }
}

#endif //AGPU_TEXTURE_FORMAT_HPP
