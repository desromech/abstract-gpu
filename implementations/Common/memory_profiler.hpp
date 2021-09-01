#ifndef AGPU_MEMORY_PROFILER_HPP
#define AGPU_MEMORY_PROFILER_HPP

namespace AgpuCommon
{
void Profile_ObjectConstruction(const char *className, const void *thisInstance);
void Profile_ObjectDestruction(const char *className, const void *thisInstance);
}

#ifndef NDEBUG
#define AgpuProfileConstructor(className) AgpuCommon::Profile_ObjectConstruction(#className, this)
#define AgpuProfileDestructor(className) AgpuCommon::Profile_ObjectDestruction(#className, this)
#else
#define AgpuProfileConstructor(className)
#define AgpuProfileDestructor(className)
#endif

#endif //AGPU_MEMORY_PROFILER_HPP