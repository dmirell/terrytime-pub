cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_library(commands_lib STATIC ${SOURCES})

target_link_libraries(
    commands_lib PUBLIC
    market_stream_lib
    analyzer_lib
    utils_lib
    events_lib
    Boost::program_options
    spdlog::spdlog
)

if (BUILD_TESTS)
   add_subdirectory(tests)
endif()