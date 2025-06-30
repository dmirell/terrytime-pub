cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_executable(analyzer_tests ${SOURCES})

target_link_libraries(
    analyzer_tests
    PRIVATE
    analyzer_lib
    market_stream_lib
    GTest::gmock
    spdlog::spdlog
)

add_test(NAME analyzer_tests COMMAND analyzer_tests)
