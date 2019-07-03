#include "sampler.hpp"
#include "constants.hpp"

namespace AgpuVulkan
{
AVkSampler::AVkSampler(const agpu::device_ref &device)
    : device(device)
{
}

AVkSampler::~AVkSampler()
{
    vkDestroySampler(deviceForVk->device, handle, nullptr);
}

agpu::sampler_ref AVkSampler::create(const agpu::device_ref &device, agpu_sampler_description *description)
{
    if(!description)
        return agpu::sampler_ref();

    VkSamplerCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    info.minFilter = mapMinFilter(description->filter);
    info.magFilter = mapMagFilter(description->filter);
    info.mipmapMode = mapMipmapMode(description->filter);
    info.addressModeU = mapAddressMode(description->address_u);
    info.addressModeV = mapAddressMode(description->address_v);
    info.addressModeW = mapAddressMode(description->address_w);
    info.minLod = description->min_lod;
    info.maxLod = description->max_lod;
    info.anisotropyEnable = description->filter == AGPU_FILTER_ANISOTROPIC;
    info.maxAnisotropy = description->maxanisotropy;
    info.mipLodBias = description->mip_lod_bias;

    info.compareEnable = description->comparison_enabled ? VK_TRUE : VK_FALSE;
    info.compareOp = mapCompareFunction(description->comparison_function);
    /*
    TODO:
    agpu_color4f border_color;
    */

    VkSampler handle;
    auto error = vkCreateSampler(deviceForVk->device, &info, nullptr, &handle);
    if(error) return agpu::sampler_ref();

    auto sampler = agpu::makeObject<AVkSampler> (device);
    sampler.as<AVkSampler> ()->handle = handle;
    return sampler;
}

} // End of namespace AgpuVulkan
