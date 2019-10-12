#ifndef AGPU_VULKAN_BUFFER_HPP
#define AGPU_VULKAN_BUFFER_HPP

#include "device.hpp"
#include "../Common/spinlock.hpp"

namespace AgpuVulkan
{
using AgpuCommon::Spinlock;

class AVkBuffer : public agpu::buffer
{
public:
    AVkBuffer(const agpu::device_ref &device);
    ~AVkBuffer();

    static agpu::buffer_ref create(const agpu::device_ref &device, agpu_buffer_description* description, agpu_pointer initial_data);

    virtual agpu_pointer mapBuffer(agpu_mapping_access flags) override;
    virtual agpu_error unmapBuffer() override;
    virtual agpu_error getDescription(agpu_buffer_description* description) override;
    virtual agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;
    virtual agpu_error readBufferData(agpu_size offset, agpu_size size, agpu_pointer data) override;

    virtual agpu_error flushWholeBuffer() override;
    virtual agpu_error invalidateWholeBuffer() override;

    agpu::device_weakref weakDevice;
    agpu_buffer_description description;

    VkBuffer handle;
    VmaAllocation allocation;

    Spinlock mappingLock;
    void *mappedPointer;
    uint32_t mapCount;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_BUFFER_HPP
