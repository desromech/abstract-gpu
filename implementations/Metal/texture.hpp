#ifndef AGPU_METAL_TEXTURE_HPP
#define AGPU_METAL_TEXTURE_HPP

#include "device.hpp"

namespace AgpuMetal
{
    
struct AMtlTexture : public agpu::texture
{
public:
    AMtlTexture(const agpu::device_ref &device);
    ~AMtlTexture();

    static agpu::texture_ref create(const agpu::device_ref &device, agpu_texture_description* description);

    virtual agpu_error getDescription(agpu_texture_description* description) override;
    virtual agpu_pointer mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d *region) override;
    virtual agpu_error unmapLevel() override;
    virtual agpu_error readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer buffer) override;
    virtual agpu_error uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data) override;
    virtual agpu_error uploadTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data) override;
    virtual agpu_error discardUploadBuffer() override;
    virtual agpu_error discardReadbackBuffer() override;
    virtual agpu_error getFullViewDescription(agpu_texture_view_description* result) override;
    virtual agpu::texture_view_ptr createView(agpu_texture_view_description* description) override;
	virtual agpu::texture_view_ptr getOrCreateFullView() override;

    agpu::device_ref device;
    agpu::texture_view_ref fullTextureView;
    agpu_texture_description description;

    id<MTLTexture> handle;

    MTLRegion getLevelRegion(int level)
    {
        MTLRegion region;
        memset(&region, 0, sizeof(region));

        region.size.width = description.width >> level;
        if (region.size.width == 0)
            region.size.width = 1;

        region.size.height = description.height >> level;
        if (description.type == AGPU_TEXTURE_1D || region.size.height == 0)
            region.size.height = 1;

        region.size.depth = description.depth >> level;
        if (description.type != AGPU_TEXTURE_3D || region.size.depth == 0)
            region.size.depth = 1;
        return region;
    }
};

} // End of namespace AgpuMetal

#endif //AGPU_METAL_TEXTURE_HPP
