cmake_minimum_required(VERSION 3.20)

add_subdirectory(time)

set(SOURCES 
    helpers.cc
    interval_timer.cc
    scoped_logger.cc
)

add_library(utils_lib STATIC ${SOURCES})

target_link_libraries(
    utils_lib PUBLIC
    utils_time_lib
    market_stream_types_lib
    spdlog::spdlog
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()