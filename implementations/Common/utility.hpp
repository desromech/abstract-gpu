#ifndef AGPU_COMMON_UTILITY_HPP
#define AGPU_COMMON_UTILITY_HPP

#include <stddef.h>

namespace AgpuCommon
{
inline size_t nextPowerOfTwo(size_t v)
{
    // https://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
    return v;
}

inline size_t alignedTo(size_t value, size_t alignment)
{
    return (value + alignment - 1) & size_t(-intptr_t(alignment));
}
} // End of namespace AgpuCommon

#endif // AGPU_COMMON_UTILITY_HPP
