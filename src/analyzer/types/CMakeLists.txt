cmake_minimum_required(VERSION 3.20)

set(SOURCES types.cc)

add_library(analyzer_types_lib STATIC ${SOURCES})

target_link_libraries(
    analyzer_types_lib PUBLIC
    utils_lib
    market_stream_types_lib
    spdlog::spdlog
)

if (BUILD_TESTS)
    # add_subdirectory(tests)
endif()