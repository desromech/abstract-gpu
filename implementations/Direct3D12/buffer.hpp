#ifndef AGPU_D3D12_BUFFER_HPP
#define AGPU_D3D12_BUFFER_HPP

#include "device.hpp"

struct _agpu_buffer : public Object<_agpu_buffer>
{
public:
    _agpu_buffer();

    void lostReferences();

    static agpu_buffer* create(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data);

    agpu_pointer mapBuffer(agpu_mapping_access flags);
    agpu_error unmapBuffer();
    agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data);

public:

    agpu_device *device;
    agpu_buffer_description description;

    ComPtr<ID3D12Resource> uploadResource;
    ComPtr<ID3D12Resource> readBackResource;
    ComPtr<ID3D12Resource> gpuResource;

    ID3D12Resource *getActualGpuBuffer();

    union
    {
        D3D12_VERTEX_BUFFER_VIEW vertexBuffer;
        D3D12_INDEX_BUFFER_VIEW indexBuffer;
        D3D12_CONSTANT_BUFFER_VIEW_DESC constantBuffer;
    } view;

private:
    agpu_error createView();
    agpu_pointer *mappedPointer;
};

#endif //AGPU_D3D12_BUFFER_HPP
