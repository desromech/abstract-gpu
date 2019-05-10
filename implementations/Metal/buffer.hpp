#ifndef AGPU_BUFFER_HPP
#define AGPU_BUFFER_HPP

#include "device.hpp"

namespace AgpuMetal
{

struct AMtlBuffer : public agpu::buffer
{
public:
    AMtlBuffer(const agpu::device_ref &device);
    ~AMtlBuffer();

    static agpu::buffer_ref create ( const agpu::device_ref &device, agpu_buffer_description* description, agpu_pointer initial_data );

    virtual agpu_pointer mapBuffer(agpu_mapping_access flags) override;
    virtual agpu_error unmapBuffer() override;
    virtual agpu_error getDescription(agpu_buffer_description* description) override;
    virtual agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;
    virtual agpu_error readBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;
    virtual agpu_error flushWholeBuffer() override;
    virtual agpu_error invalidateWholeBuffer() override;

    agpu::device_ref device;
    agpu_buffer_description description;
    id<MTLBuffer> handle;
};

} // End of namespace AgpuMetal

#endif //AGPU_BUFFER_HPP
