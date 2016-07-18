#ifndef AGPU_VULKAN_BUFFER_HPP
#define AGPU_VULKAN_BUFFER_HPP

#include "device.hpp"

struct _agpu_buffer : public Object<_agpu_buffer>
{
    _agpu_buffer(agpu_device *device);
    void lostReferences();

    static agpu_buffer* create(agpu_device* device, agpu_buffer_description* description, agpu_pointer initial_data);

    agpu_pointer map(agpu_mapping_access flags);
    agpu_error unmap();
    agpu_error getBufferDescription(agpu_buffer_description* description);
    agpu_error uploadBufferData(agpu_size offset, agpu_size size, agpu_pointer data);
    agpu_error readBufferData(agpu_size offset, agpu_size size, agpu_pointer data);

    agpu_error flushWholeBuffer();
    agpu_error invalidateWholeBuffer();

    VkBuffer getDrawBuffer()
    {
        if (gpuBuffer)
            return gpuBuffer;
        return uploadBuffer;
    }
    VkIndexType getIndexType()
    {
        switch (description.stride)
        {
        default:
        case 2: return VK_INDEX_TYPE_UINT16;
        case 4: return VK_INDEX_TYPE_UINT32;
        }
    }

    agpu_device *device;
    agpu_buffer_description description;

    VkBuffer uploadBuffer;
    VkDeviceMemory uploadBufferMemory;
    VkBuffer gpuBuffer;
    VkDeviceMemory gpuBufferMemory;
    VkMemoryRequirements memoryRequirements;

    std::mutex mapMutex;
    void *mappedPointer;
    int mapCount;
};
#endif //AGPU_VULKAN_BUFFER_HPP
