#ifndef AGPU_TEXTURE_HPP
#define AGPU_TEXTURE_HPP

#include "device.hpp"

namespace AgpuVulkan
{

class AVkTexture : public agpu::texture
{
public:
    AVkTexture(const agpu::device_ref &device);
    ~AVkTexture();

    static agpu::texture_ref create(const agpu::device_ref &device, agpu_texture_description *description);
    static agpu::texture_ref createFromImage(const agpu::device_ref &device, agpu_texture_description *description, VkImage image);

    virtual agpu_error getDescription(agpu_texture_description* description) override;
    virtual agpu_error getFullViewDescription(agpu_texture_view_description *viewDescription) override;
    virtual agpu_pointer mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d *region) override;
    virtual agpu_error unmapLevel() override;
    virtual agpu_error readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer) override;
    virtual agpu_error uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data) override;
    virtual agpu_error uploadTextureSubData ( agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data ) override;
    virtual agpu::texture_view_ptr createView(agpu_texture_view_description* description) override;
	virtual agpu::texture_view_ptr getOrCreateFullView() override;

    agpu::device_ref device;
    agpu_texture_description description;
    VkImage image;
    VkImageAspectFlags imageAspect;
    VmaAllocation memory;
    bool owned;
    agpu::texture_view_ref fullTextureView;

    VkExtent3D getLevelExtent(int level);
    void computeBufferImageTransferLayout(int level, VkSubresourceLayout *layout, VkBufferImageCopy *copy);
};

} // End of namespace AgpuVulkan

#endif //AGPU_TEXTURE_HPP
