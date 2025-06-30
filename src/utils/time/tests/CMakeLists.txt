cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_executable(utils_time_tests ${SOURCES})

target_link_libraries(
    utils_time_tests
    PRIVATE
    utils_time_lib
    utils_lib
    GTest::gmock
    spdlog::spdlog
)

add_test(NAME utils_tests COMMAND utils_time_tests)
