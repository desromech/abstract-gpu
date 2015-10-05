set(AGPU_SOURCES
    loader.cpp
    redirection.cpp
)

add_definitions(-DAGPU_BUILD)

add_library(Agpu SHARED ${AGPU_SOURCES})
target_link_libraries(Agpu)
