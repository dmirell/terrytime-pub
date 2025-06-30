cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_executable(commands_tests ${SOURCES})

target_link_libraries(
    commands_tests
    PRIVATE
    commands_lib
    GTest::gtest
    spdlog::spdlog
)

add_test(NAME commands_tests COMMAND commands_tests)
