#include "state_tracker_cache.hpp"
#include "state_tracker.hpp"

namespace AgpuCommon
{
StateTrackerCache::StateTrackerCache(const agpu::device_ref &device, uint32_t queueFamilyType)
    : device(device), queueFamilyType(queueFamilyType)
{
}

StateTrackerCache::~StateTrackerCache()
{
}

agpu::state_tracker_cache_ref StateTrackerCache::create(const agpu::device_ref &device, uint32_t queueFamilyType)
{
    return agpu::makeObject<StateTrackerCache> (device, queueFamilyType);
}

agpu::state_tracker_ptr StateTrackerCache::createStateTracker(agpu_command_list_type type, const agpu::command_queue_ref & command_queue)
{
    auto commandAllocator = agpu::command_allocator_ref(device->createCommandAllocator(type, command_queue));
    if(!commandAllocator) return nullptr;

    return DirectStateTracker::create(refFromThis<agpu::state_tracker_cache> (), device, type, command_queue, commandAllocator, true).disown();
}

agpu::state_tracker_ptr StateTrackerCache::createStateTrackerWithCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, const agpu::command_allocator_ref & command_allocator)
{
    return DirectStateTracker::create(refFromThis<agpu::state_tracker_cache> (), device, type, command_queue, command_allocator, false).disown();
}

agpu::state_tracker_ptr StateTrackerCache::createStateTrackerWithFrameBuffering(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, agpu_uint framebuffering_count)
{
    return FrameBufferredStateTracker::create(refFromThis<agpu::state_tracker_cache> (), device, type, command_queue, framebuffering_count).disown();
}

} // End of namespace AgpuCommon
