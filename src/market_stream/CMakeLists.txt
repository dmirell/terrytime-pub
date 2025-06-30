cmake_minimum_required(VERSION 3.20)

add_subdirectory(types)

set(SOURCES 
    binapi_client.cc 
    order_book_stream_forwarder.cc 
    trades_stream_forwarder.cc 
    market_stream_forwarder.cc 
    market_stream_saver.cc
    saved_market_stream_forwarder.cc
    market_stream_printer.cc
)

add_library(market_stream_lib STATIC ${SOURCES})

target_link_libraries(
    market_stream_lib PUBLIC
    market_stream_types_lib
    events_lib
    analyzer_lib
    utils_time_lib
    BinAPI::binapilib
    Boost::serialization
    Boost::filesystem
    spdlog::spdlog
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()
