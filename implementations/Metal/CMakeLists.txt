set(AGPU_Metal_SOURCES
    common.hpp
    device.hpp
    device.cpp
    icd.c
    object.hpp
    platform.cpp
    platform.hpp
    stubs.cpp
)

add_definitions(-DAGPU_BUILD)

add_library(AgpuMetal SHARED ${AGPU_Metal_SOURCES})
target_link_libraries(AgpuMetal)
