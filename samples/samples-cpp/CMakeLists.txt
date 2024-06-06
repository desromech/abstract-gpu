set(SampleCommon_SRC
    SampleBase.cpp
    SampleBase.hpp
)

add_library(SampleCppCommon STATIC ${SampleCommon_SRC})
target_link_libraries(SampleCppCommon ${AGPU_SAMPLES_LIBS})

add_executable(Sample-Cpp-Compute1 SampleCompute1.cpp)
target_link_libraries(Sample-Cpp-Compute1 SampleCppCommon)

add_executable(Sample-Cpp-1 Sample1.cpp)
target_link_libraries(Sample-Cpp-1 SampleCppCommon)

add_executable(Sample-Cpp-2 Sample2.cpp)
target_link_libraries(Sample-Cpp-2 SampleCppCommon)

add_executable(Sample-Cpp-3 Sample3.cpp)
target_link_libraries(Sample-Cpp-3 SampleCppCommon)

add_executable(Sample-Cpp-4 Sample4.cpp)
target_link_libraries(Sample-Cpp-4 SampleCppCommon)

add_executable(Sample-Cpp-StateTracker-1 SampleStateTracker1.cpp)
target_link_libraries(Sample-Cpp-StateTracker-1 SampleCppCommon)

add_executable(Sample-Cpp-Immediate-1 SampleImmediate1.cpp)
target_link_libraries(Sample-Cpp-Immediate-1 SampleCppCommon)

add_executable(Sample-Cpp-Immediate-2 SampleImmediate2.cpp)
target_link_libraries(Sample-Cpp-Immediate-2 SampleCppCommon)

add_executable(Sample-Cpp-WindowScraper SampleWindowScraper.cpp)
target_link_libraries(Sample-Cpp-WindowScraper SampleCppCommon)
