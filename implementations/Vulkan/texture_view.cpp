#include "texture_view.hpp"

namespace AgpuVulkan
{
AVkTextureView::AVkTextureView(const agpu::device_ref &device)
    : device(device)
{
}

AVkTextureView::~AVkTextureView()
{
    vkDestroyImageView(deviceForVk->device, handle, nullptr);
}

agpu::texture_view_ref AVkTextureView::create(const agpu::device_ref &device, const agpu::texture_ref &texture, VkImageView handle, VkImageLayout imageLayout, const agpu_texture_view_description &description)
{
    auto result = agpu::makeObject<AVkTextureView> (device);
    auto view = result.as<AVkTextureView> ();
    view->handle = handle;
    view->texture = texture;
    view->imageLayout = imageLayout;
    view->description = description;
    return result;
}

agpu::texture_ptr AVkTextureView::getTexture()
{
    return texture.lock().disown();
}

} // End of namespace AgpuVulkan
