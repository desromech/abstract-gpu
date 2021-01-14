#ifndef AGPU_D3D12_TEXTURE_HPP
#define AGPU_D3D12_TEXTURE_HPP

#include "device.hpp"

namespace AgpuD3D12
{

class ADXTexture : public agpu::texture
{
public:
    ADXTexture(const agpu::device_ref &cdevice);
    ~ADXTexture();

    static agpu::texture_ref create(const agpu::device_ref &device, agpu_texture_description* description);
    static agpu::texture_ref createFromResource(const agpu::device_ref &device, agpu_texture_description* description, const ComPtr<ID3D12Resource> &resource);

    virtual agpu_error getDescription(agpu_texture_description* description) override;

    virtual agpu_pointer mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags, agpu_region3d* region) override;
    virtual agpu_error unmapLevel() override;

    virtual agpu_error readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data) override;
	virtual agpu_error readTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_region3d* sourceRegion, agpu_size3d* destSize, agpu_pointer buffer) override;
	virtual agpu_error uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data) override;
    virtual agpu_error uploadTextureSubData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_size3d* sourceSize, agpu_region3d* destRegion, agpu_pointer data) override;

    virtual agpu_error getFullViewDescription(agpu_texture_view_description *description) override;
    virtual agpu::texture_view_ptr createView(agpu_texture_view_description* viewDescription) override;
	virtual agpu::texture_view_ptr getOrCreateFullView() override;

public:
    UINT subresourceIndexFor(agpu_uint level, agpu_uint arrayIndex);
    bool isArray();
    void releaseTextureHandle();

    agpu::device_ref device;
    agpu_texture_description description;

    D3D12_RESOURCE_DESC resourceDescription;

    ComPtr<ID3D12Resource> resource;
    ComPtr<D3D12MA::Allocation> allocation;

private:
    agpu_uint mapCount;
    agpu_int mappedLevel;
    agpu_uint mappedArrayIndex;
    agpu_pointer mappedPointer;
    agpu_mapping_access mappingFlags;
	agpu::texture_view_ref fullTextureView;
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_TEXTURE_HPP
