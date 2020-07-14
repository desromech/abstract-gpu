#include "descriptor_pool.hpp"
#include <assert.h>
#include <algorithm>

namespace AgpuVulkan
{

void AVkDescriptorSetPoolAllocation::free()
{
    owner->free(this);
}

AVkDescriptorSetPoolBlock::AVkDescriptorSetPoolBlock()
{
    poolHandle = VK_NULL_HANDLE;
}

AVkDescriptorSetPoolBlock::~AVkDescriptorSetPoolBlock()
{
    auto &device = owner->device;
    for(auto allocation : allocationList)
        delete allocation;

    if(poolHandle != VK_NULL_HANDLE)
        vkDestroyDescriptorPool(deviceForVk->device, poolHandle, nullptr);
}

AVkDescriptorSetPoolAllocation *AVkDescriptorSetPoolBlock::allocate()
{
    assert(this == owner->freeBlocks.last());
    if(freeList.empty())
    {
        if(remainingAllocationCount == 0)
            return nullptr;

        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.descriptorPool = poolHandle;
        allocateInfo.descriptorSetCount = 1;
        allocateInfo.pSetLayouts = &owner->setDescription.descriptorSetLayout;

        VkDescriptorSet descriptorSet;
        auto &device = owner->device;
        auto error = vkAllocateDescriptorSets(deviceForVk->device, &allocateInfo, &descriptorSet);
        if (error)
        {
            printError("Failed to allocate descriptor set.\n");
            return nullptr;
        }

        auto newAllocation = new AVkDescriptorSetPoolAllocation;
        newAllocation->descriptorSet = descriptorSet;
        newAllocation->owner = this;
        allocationList.push_back(newAllocation);
        freeList.push_back(newAllocation);
    }

    auto result = freeList.back();
    freeList.pop_back();
    if(freeList.empty() && remainingAllocationCount == 0)
        owner->allocationBlockCompleted(shared_from_this());

    return result;
}

void AVkDescriptorSetPoolBlock::free(AVkDescriptorSetPoolAllocation *allocation)
{
    auto isNewFreeBlock = freeList.empty() && remainingAllocationCount == 0;
    freeList.push_back(allocation);
    if(isNewFreeBlock)
        owner->freeBlocks.push_back(shared_from_this());
}

AVkDescriptorSetPool::AVkDescriptorSetPool()
{
}

AVkDescriptorSetPool::~AVkDescriptorSetPool()
{
    allocationBlocks.clear();
    freeBlocks.clear();
    if(setDescription.descriptorSetLayout != VK_NULL_HANDLE)
        vkDestroyDescriptorSetLayout(deviceForVk->device, setDescription.descriptorSetLayout, nullptr);
}

AVkDescriptorSetPoolAllocation *AVkDescriptorSetPool::allocate()
{
    std::unique_lock<std::mutex> l(mutex);

    if(freeBlocks.empty())
    {
        VkDescriptorPoolCreateInfo poolCreateInfo = {};
        poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolCreateInfo.maxSets = setDescription.maxBindings;
        poolCreateInfo.poolSizeCount = (uint32_t)elementSizes.size();
        poolCreateInfo.pPoolSizes = &elementSizes[0];
        poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        VkDescriptorPool poolHandle;
        auto error = vkCreateDescriptorPool(deviceForVk->device, &poolCreateInfo, nullptr, &poolHandle);
        if (error)
            return nullptr;

        auto newBlock = std::make_shared<AVkDescriptorSetPoolBlock> ();
        newBlock->owner = this;
        newBlock->poolHandle = poolHandle;
        newBlock->remainingAllocationCount = poolCreateInfo.poolSizeCount;
        allocationBlocks.push_back(newBlock);
        freeBlocks.push_back(newBlock);
    }

    return freeBlocks.back()->allocate();
}

void AVkDescriptorSetPool::allocationBlockCompleted(AVkDescriptorSetPoolBlockPtr block)
{
    auto blockPosition = std::find(freeBlocks.begin(), freeBlocks.end(), block);
    if(blockPosition != freeBlocks.end())
        freeBlocks.erase(blockPosition);
}

void AVkDescriptorSetPool::free(AVkDescriptorSetPoolAllocation *allocation)
{
    std::unique_lock<std::mutex> l(mutex);
    allocation->owner->free(allocation);
}

} // End of namespace AgpuVulkan
