cmake_minimum_required(VERSION 3.20)

add_subdirectory(utils)
add_subdirectory(events)
add_subdirectory(market_stream)
add_subdirectory(commands)
add_subdirectory(analyzer)

add_library(terry_lib STATIC main.cc)

target_link_libraries(
    terry_lib PUBLIC
    commands_lib
    spdlog::spdlog
)
