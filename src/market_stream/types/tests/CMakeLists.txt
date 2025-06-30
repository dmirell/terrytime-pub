cmake_minimum_required(VERSION 3.20)

file(GLOB SOURCES "*.cc")

add_executable(market_stream_types_tests ${SOURCES})

target_link_libraries(
    market_stream_types_tests
    PRIVATE
    market_stream_types_lib
    GTest::gtest
    spdlog::spdlog
)

add_test(NAME market_stream_types_tests COMMAND market_stream_types_tests)
