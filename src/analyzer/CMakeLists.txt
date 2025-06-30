cmake_minimum_required(VERSION 3.20)

set(SOURCES 
    observable_units.cc
    order_book_snapshot_provider.cc
    benchmark_manager.cc
    benchmark_orchestrator.cc
    dummy_trading_strategy.cc
    market_analyzer.cc
    order_plan_manager.cc
    order_manager.cc
    real_market_emulator.cc
    benchmark_data_collector.cc
)

add_subdirectory(types)

add_library(analyzer_lib STATIC ${SOURCES})

target_link_libraries(
  analyzer_lib PUBLIC
    market_stream_lib
    analyzer_types_lib
    BinAPI::binapilib
    utils_lib
    events_lib
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()