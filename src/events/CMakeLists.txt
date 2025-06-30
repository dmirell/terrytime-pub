cmake_minimum_required(VERSION 3.20)

set(SOURCES event_hub.cc)

add_library(events_lib STATIC ${SOURCES})

target_link_libraries(
    events_lib
    PUBLIC
    market_stream_types_lib
    analyzer_types_lib
    spdlog::spdlog
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()