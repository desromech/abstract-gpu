#ifndef AGPU_TEXTURE_FORMAT_HPP
#define AGPU_TEXTURE_FORMAT_HPP

#include <AGPU/agpu.h>

inline GLenum findTextureTarget(agpu_texture_description *description)
{
    assert(description);
    switch(description->type)
    {
    case AGPU_TEXTURE_1D:
        if(description->depthOrArraySize > 0)
            return GL_TEXTURE_1D_ARRAY;
        else
            return GL_TEXTURE_1D;
    case AGPU_TEXTURE_2D:
        if(description->depthOrArraySize > 0)
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

inline GLenum mapTextureFormat(agpu_texture_format format)
{
    switch(format)
    {
    default:
        abort();
    }
}

#endif //AGPU_TEXTURE_FORMAT_HPP
