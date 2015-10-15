#ifndef AGPU_D3D12_TEXTURE_HPP
#define AGPU_D3D12_TEXTURE_HPP

#include "device.hpp"

struct _agpu_texture : public Object<_agpu_texture>
{
public:
    _agpu_texture();

    void lostReferences();

    static agpu_texture* create(agpu_device* device, agpu_texture_description* description);
    static agpu_texture* createFromResource(agpu_device* device, agpu_texture_description* description, const ComPtr<ID3D12Resource> &resource);

    agpu_error readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data);
    agpu_error uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data);

    agpu_error discardUploadBuffer();
    agpu_error discardReadbackBuffer();

public:
    UINT subresourceIndexFor(agpu_uint level, agpu_uint arrayIndex);

    agpu_device* device;
    agpu_texture_description description;

    D3D12_RESOURCE_DESC resourceDescription;
    ComPtr<ID3D12Resource> gpuResource;
    ComPtr<ID3D12Resource> uploadResource;
    ComPtr<ID3D12Resource> readbackResource;

private:


};

#endif //AGPU_D3D12_TEXTURE_HPP
