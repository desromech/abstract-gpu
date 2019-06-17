set(SampleCommon_SRC
    SampleBase.cpp
    SampleBase.hpp
)

add_library(SampleCppCommon STATIC ${SampleCommon_SRC})
target_link_libraries(SampleCppCommon ${AGPU_SAMPLES_LIBS})

add_executable(Sample-Cpp-1 Sample1.cpp)
target_link_libraries(Sample-Cpp-1 SampleCppCommon)

add_executable(Sample-Cpp-2 Sample2.cpp)
target_link_libraries(Sample-Cpp-2 SampleCppCommon)

add_executable(Sample-Cpp-3 Sample3.cpp)
target_link_libraries(Sample-Cpp-3 SampleCppCommon)

add_executable(Sample-Cpp-4 Sample4.cpp)
target_link_libraries(Sample-Cpp-4 SampleCppCommon)

add_executable(Sample-Cpp-5 Sample5.cpp)
target_link_libraries(Sample-Cpp-5 SampleCppCommon)
