cmake_minimum_required(VERSION 3.20)

set(SOURCES 
    elapsed_time_meter.cc
    mock_time_provider.cc
    time_provider.cc
    global_clock.cc
)

add_library(utils_time_lib STATIC ${SOURCES})

target_link_libraries(
    utils_time_lib PUBLIC
    spdlog::spdlog
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()