#ifndef AGPU_VULKAN_DESCRIPTOR_POOL_HPP
#define AGPU_VULKAN_DESCRIPTOR_POOL_HPP

#include "device.hpp"
#include "shader_signature_builder.hpp"
#include <memory>
#include <vector>

namespace AgpuVulkan
{
class AVkDescriptorSetPoolAllocation;
class AVkDescriptorSetPoolBlock;
class AVkDescriptorSetPool;

typedef std::shared_ptr<AVkDescriptorSetPoolBlock> AVkDescriptorSetPoolBlockPtr;
typedef std::shared_ptr<AVkDescriptorSetPool> AVkDescriptorSetPoolPtr;

class AVkDescriptorSetPoolAllocation
{
public:
    void free();

    VkDescriptorSet descriptorSet;
    AVkDescriptorSetPoolBlock *owner;
};

class AVkDescriptorSetPoolBlock : public std::enable_shared_from_this<AVkDescriptorSetPoolBlock>
{
public:
    AVkDescriptorSetPoolBlock();
    ~AVkDescriptorSetPoolBlock();

    AVkDescriptorSetPoolAllocation *allocate();
    void free(AVkDescriptorSetPoolAllocation *allocation);

    AVkDescriptorSetPool *owner;
    VkDescriptorPool poolHandle;
    uint32_t remainingAllocationCount;
    std::vector<AVkDescriptorSetPoolAllocation*> allocationList;
    std::vector<AVkDescriptorSetPoolAllocation*> freeList;
};

class AVkDescriptorSetPool
{
public:
    AVkDescriptorSetPool();
    ~AVkDescriptorSetPool();

    agpu::device_ref device;
    std::mutex mutex;

    AVkDescriptorSetPoolAllocation *allocate();
    void allocationBlockCompleted(AVkDescriptorSetPoolBlockPtr block);
    void free(AVkDescriptorSetPoolAllocation *allocation);

    ShaderSignatureElementDescription setDescription;
    std::vector<AVkDescriptorSetPoolBlockPtr> allocationBlocks;
    std::vector<AVkDescriptorSetPoolBlockPtr> freeBlocks;
    std::vector<VkDescriptorPoolSize> elementSizes;
};

} // End of namespace AgpuVulkan

#endif //AGPU_VULKAN_DESCRIPTOR_POOL_HPP
