#include "sampler.hpp"
#include "constants.hpp"
#include "../Common/memory_profiler.hpp"
#include <algorithm>

namespace AgpuMetal
{
inline MTLSamplerMinMagFilter mapMinFilter(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST:return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR: return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST: return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:  return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST: return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:  return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:  return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:   return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_ANISOTROPIC:                           return MTLSamplerMinMagFilterLinear;
    }
}

inline MTLSamplerMinMagFilter mapMagFilter(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST: return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:  return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:  return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:   return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:  return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:   return MTLSamplerMinMagFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:   return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:    return MTLSamplerMinMagFilterLinear;
    case AGPU_FILTER_ANISOTROPIC:                            return MTLSamplerMinMagFilterLinear;
    }
}

inline MTLSamplerMipFilter mapMipmapMode(agpu_filter filter)
{
    switch (filter)
    {
    default:
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_NEAREST: return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_NEAREST_MIPMAP_LINEAR:  return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_NEAREST:  return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_NEAREST_MAG_LINEAR_MIPMAP_LINEAR:   return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_NEAREST:  return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_NEAREST_MIPMAP_LINEAR:   return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_NEAREST:   return MTLSamplerMipFilterNearest;
    case AGPU_FILTER_MIN_LINEAR_MAG_LINEAR_MIPMAP_LINEAR:    return MTLSamplerMipFilterLinear;
    case AGPU_FILTER_ANISOTROPIC:                            return MTLSamplerMipFilterLinear;
    }
}

inline MTLSamplerAddressMode mapAddressMode(agpu_texture_address_mode mode)
{
    switch (mode)
    {
    default:
    case AGPU_TEXTURE_ADDRESS_MODE_WRAP:    return MTLSamplerAddressModeRepeat;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR:  return MTLSamplerAddressModeMirrorRepeat;
    case AGPU_TEXTURE_ADDRESS_MODE_CLAMP:   return MTLSamplerAddressModeClampToEdge;
    case AGPU_TEXTURE_ADDRESS_MODE_BORDER:  return MTLSamplerAddressModeClampToZero;
    case AGPU_TEXTURE_ADDRESS_MODE_MIRROR_ONCE: return MTLSamplerAddressModeMirrorClampToEdge;
    }
}

AMtlSampler::AMtlSampler(const agpu::device_ref &device, id<MTLSamplerState> handle)
    : device(device), handle(handle)
{
    AgpuProfileConstructor(AMtlSampler);
}

AMtlSampler::~AMtlSampler()
{
    AgpuProfileDestructor(AMtlSampler);
}

agpu::sampler_ref AMtlSampler::create(const agpu::device_ref &device, agpu_sampler_description *description)
{
    auto descriptor = [MTLSamplerDescriptor new];
    descriptor.sAddressMode = mapAddressMode(description->address_u);
    descriptor.tAddressMode = mapAddressMode(description->address_v);
    descriptor.rAddressMode = mapAddressMode(description->address_w);
    descriptor.minFilter = mapMinFilter(description->filter);
    descriptor.magFilter = mapMinFilter(description->filter);
    descriptor.mipFilter = mapMipmapMode(description->filter);
    descriptor.lodMinClamp = description->min_lod;
    descriptor.lodMaxClamp = description->max_lod;
    descriptor.maxAnisotropy = description->filter == AGPU_FILTER_ANISOTROPIC ? description->maxanisotropy : 1.0f;
    descriptor.normalizedCoordinates = YES;
    descriptor.compareFunction = mapCompareFunction(description->comparison_function);

    auto handle = [deviceForMetal->device newSamplerStateWithDescriptor: descriptor];
    if(!handle)
        return agpu::sampler_ref();

    return agpu::makeObject<AMtlSampler> (device, handle);
}

} // namespace AgpuMetal
