#ifndef AGPU_D3D12_BUFFER_HPP
#define AGPU_D3D12_BUFFER_HPP

#include "device.hpp"
#include "../Common/spinlock.hpp"

namespace AgpuD3D12
{
using AgpuCommon::Spinlock;

class ADXBuffer : public agpu::buffer
{
public:
    ADXBuffer(const agpu::device_ref &device);
    ~ADXBuffer();

    static agpu::buffer_ref create(const agpu::device_ref &device, agpu_buffer_description* description, agpu_pointer initial_data);

    virtual agpu_pointer mapBuffer(agpu_mapping_access flags) override;
    virtual agpu_error unmapBuffer() override;
    virtual agpu_error getDescription(agpu_buffer_description* description) override;

    virtual agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;
    virtual agpu_error readBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;

    virtual agpu_error flushWholeBuffer() override;
	virtual agpu_error invalidateWholeBuffer() override;

public:
    agpu::device_weakref weakDevice;
    agpu_buffer_description description;

    ComPtr<ID3D12Resource> resource;
    ComPtr<D3D12MA::Allocation> allocation;

    D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress;
    agpu_error createIndexBufferView(D3D12_INDEX_BUFFER_VIEW *outView, agpu_size offset, agpu_size index_size);
    agpu_error createVertexBufferView(D3D12_VERTEX_BUFFER_VIEW *outView, agpu_size offset, agpu_size stride);
    agpu_error createConstantBufferViewDescription(D3D12_CONSTANT_BUFFER_VIEW_DESC *outView, agpu_size offset, agpu_size size);
    agpu_error createUAVDescription(D3D12_UNORDERED_ACCESS_VIEW_DESC *outView, agpu_size offset, agpu_size size);

private:
    agpu_error createView();
};

} // End of namespace AgpuD3D12

#endif //AGPU_D3D12_BUFFER_HPP
