cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_executable(market_stream_tests ${SOURCES})

target_include_directories(
    market_stream_tests PRIVATE helpers
)

target_link_libraries(
    market_stream_tests
    PRIVATE
    market_stream_lib
    GTest::gmock
    spdlog::spdlog
)

add_test(NAME market_stream_tests COMMAND market_stream_tests)
