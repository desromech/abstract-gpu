#ifndef AGPU_TEXTURE_VIEW_HPP
#define AGPU_TEXTURE_VIEW_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkTextureView : public agpu::texture_view
{
public:
    AVkTextureView(const agpu::device_ref &device);
    ~AVkTextureView();

    static agpu::texture_view_ref create(const agpu::device_ref &device, const agpu::texture_ref &texture, VkImageView handle, VkImageLayout imageLayout, const agpu_texture_view_description &description);

    virtual agpu::texture_ptr getTexture() override;

    agpu::device_ref device;
    agpu::texture_weakref texture;

    VkImageLayout imageLayout;
    VkImageView handle;
    agpu_texture_view_description description;
};

} // End of namespace AgpuVulkan

#endif //AGPU_TEXTURE_VIEW_HPP
