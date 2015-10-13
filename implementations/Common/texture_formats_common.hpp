#ifndef AGPU_TEXTURE_FORMATS_COMMON_HPP
#define AGPU_TEXTURE_FORMATS_COMMON_HPP

inline size_t pixelSizeOfTextureFormat(agpu_texture_format format)
{
    switch(format)
    {
    case AGPU_TEXTURE_FORMAT_R8G8B8A8_UNORM: return 4;
    case AGPU_TEXTURE_FORMAT_D16_UNORM: return 2;
	case AGPU_TEXTURE_FORMAT_D32_FLOAT: return 4;
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

#endif //AGPU_TEXTURE_FORMATS_COMMON_HPP
