#ifndef AGPU_VULKAN_BUFFER_HPP
#define AGPU_VULKAN_BUFFER_HPP

#include "device.hpp"

namespace AgpuVulkan
{

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

    VkBuffer getDrawBuffer()
    {
        if (gpuBuffer)
            return gpuBuffer;
        return uploadBuffer;
    }

    agpu::device_weakref weakDevice;
    agpu_buffer_description description;

    VkBuffer uploadBuffer;
    VkDeviceMemory uploadBufferMemory;
    VkBuffer gpuBuffer;
    VkDeviceMemory gpuBufferMemory;

    std::mutex mapMutex;
    void *mappedPointer;
    int mapCount;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_BUFFER_HPP
