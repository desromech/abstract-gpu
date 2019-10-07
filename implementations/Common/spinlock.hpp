#ifndef AGPU_COMMON_SPINLOCK_HPP
#define AGPU_COMMON_SPINLOCK_HPP

#include <atomic>

namespace AgpuCommon
{

/**
 * Simple implementation of a spinlock. This should be used sparingly, for
 * example for mapping single buffers. The motivation for using a spin-lock is
 * to avoid exhausting operating system handles for mutexes.
 */
class Spinlock
{
public:
    void lock()
    {
        while(islocked.test_and_set(std::memory_order_acquire))
            ;
    }

    void unlock()
    {
        islocked.clear(std::memory_order_release);
    }

private:
    std::atomic_flag islocked = ATOMIC_FLAG_INIT;
};

} // End of namespace AgpuCommon

#endif //AGPU_COMMON_SPINLOCK_HPP
