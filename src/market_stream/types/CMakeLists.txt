cmake_minimum_required(VERSION 3.20)

set(SOURCES types.cc)

add_library(market_stream_types_lib STATIC ${SOURCES})

target_link_libraries(
    market_stream_types_lib PUBLIC
    BinAPI::binapilib
    Boost::serialization
    spdlog::spdlog
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()