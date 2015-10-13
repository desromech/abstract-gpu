#ifndef AGPU_TEXTURE_FORMATS_HPP
#define AGPU_TEXTURE_FORMATS_HPP

#include <AGPU/agpu.h>
#include "../Common/texture_formats_common.hpp"

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

#endif //AGPU_TEXTURE_FORMATS_HPP
