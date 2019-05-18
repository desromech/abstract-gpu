#ifndef AGPU_STATE_TRACKER_CACHE_HPP
#define AGPU_STATE_TRACKER_CACHE_HPP

#include <AGPU/agpu_impl.hpp>
#include <unordered_map>

namespace AgpuCommon
{

/**
 * I am a cache for the on the fly generated pipeline state objects that are
 * required by an state tracker.
 */
class StateTrackerCache : public agpu::state_tracker_cache
{
public:
    StateTrackerCache(const agpu::device_ref &device, uint32_t queueFamilyType);
    ~StateTrackerCache();

    static agpu::state_tracker_cache_ref create(const agpu::device_ref &device, uint32_t queueFamilyType);

    virtual agpu::state_tracker_ptr createStateTracker(agpu_command_list_type type, const agpu::command_queue_ref & command_queue) override;
	virtual agpu::state_tracker_ptr createStateTrackerWithCommandAllocator(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, const agpu::command_allocator_ref & command_allocator) override;
	virtual agpu::state_tracker_ptr createStateTrackerWithFrameBuffering(agpu_command_list_type type, const agpu::command_queue_ref & command_queue, agpu_uint framebuffering_count) override;

    agpu::device_ref device;
    uint32_t queueFamilyType;
private:
};

} // End of namespace AgpuCommon

#endif //AGPU_STATE_TRACKER_CACHE_HPP
