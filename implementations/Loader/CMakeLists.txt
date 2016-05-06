set(AGPU_SOURCES
    loader.cpp
    redirection.cpp
)

add_definitions(-DAGPU_BUILD)
if(X11_LIBRARY)
    add_definitions(-DHAS_XLIB)
endif()

add_library(Agpu SHARED ${AGPU_SOURCES})
target_link_libraries(Agpu ${LOADER_OS_LIBRARIES})
