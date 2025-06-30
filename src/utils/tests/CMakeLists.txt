cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_executable(utils_tests ${SOURCES})

target_link_libraries(
    utils_tests
    PRIVATE
    utils_lib
    GTest::gtest
    spdlog::spdlog
)

add_test(NAME utils_tests COMMAND utils_tests)
