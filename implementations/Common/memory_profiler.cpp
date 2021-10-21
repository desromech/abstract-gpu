#include "memory_profiler.hpp"
#include <stdio.h>

namespace AgpuCommon
{

static bool IsProfilingEnabled()
{
    return false;
}

void Profile_ObjectConstruction(const char *className, const void *thisInstance)
{
    if(IsProfilingEnabled())
        fprintf(stderr, "AgpuConstructor: %s: %p\n", className, thisInstance);
}

void Profile_ObjectDestruction(const char *className, const void *thisInstance)
{
    if(IsProfilingEnabled())
        fprintf(stderr, "AgpuDestructor: %s: %p\n", className, thisInstance);
}

} // End of namespace AgpuCommon