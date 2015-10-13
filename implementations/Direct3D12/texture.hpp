#ifndef AGPU_D3D12_TEXTURE_HPP
#define AGPU_D3D12_TEXTURE_HPP

#include "device.hpp"

struct _agpu_texture : public Object<_agpu_texture>
{
public:
    _agpu_texture();

    void lostReferences();

    static agpu_texture* create(agpu_device* device, agpu_texture_description* description, agpu_pointer initialData);
    static agpu_texture* createFromResource(agpu_device* device, agpu_texture_description* description, const ComPtr<ID3D12Resource> &resource);

    agpu_pointer mapLevel(agpu_int level, agpu_int arrayIndex, agpu_mapping_access flags);
    agpu_error unmapLevel();
    agpu_error readTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data);
    agpu_error uploadTextureData(agpu_int level, agpu_int arrayIndex, agpu_int pitch, agpu_int slicePitch, agpu_pointer data);

public:
    agpu_device* device;
    agpu_texture_description description;

    D3D12_RESOURCE_DESC resourceDescription;
    ComPtr<ID3D12Resource> gpuResource;
    ComPtr<ID3D12Resource> uploadResource;
    ComPtr<ID3D12Resource> readbackResource;

private:
    size_t getPixelSize();
    size_t pitchOfLevel(int level);
    size_t slicePitchOfLevel(int level);
    size_t sizeOfLevel(int level);

};

#endif //AGPU_D3D12_TEXTURE_HPP
