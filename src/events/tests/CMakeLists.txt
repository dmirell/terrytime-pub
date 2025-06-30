cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_executable(event_hub_tests ${SOURCES})

target_link_libraries(
    event_hub_tests
    PRIVATE
    events_lib
    utils_lib
    analyzer_lib
    GTest::gmock
    spdlog::spdlog
)

add_test(NAME event_hub_tests COMMAND event_hub_tests)
