#include "commands/command_strategy_test_handler.h"

#include <spdlog/spdlog.h>

#include <boost/filesystem.hpp>
#include <chrono>
#include <fstream>
#include <iostream>

#include "analyzer/benchmark_data_collector.h"
#include "analyzer/benchmark_orchestrator.h"
#include "analyzer/dummy_trading_strategy.h"
#include "analyzer/market_analyzer.h"
#include "analyzer/order_book_snapshot_provider.h"
#include "analyzer/order_manager.h"
#include "analyzer/order_plan_manager.h"
#include "analyzer/real_market_emulator.h"
#include "events/event_hub.h"
#include "market_stream/saved_market_stream_forwarder.h"

namespace commands {

namespace {
const auto gStrategyOptionName = "strategy";
const auto gNoTsJumpOptionName = "no-ts-jump";
const auto gOutputJsonOptionName = "output-json-dir";
const auto gInputStreamDirOptionName = "stream-dir";

const auto gOutputJsonFileName = "strategy_test_result.json";
}  // namespace

CommandStrategyTestHandler::CommandStrategyTestHandler(int argc, const char* argv[]) {
  spdlog::info("command parsing...");
  po::variables_map opts_map;
  try {
    // Define options specific to the "strat test" command
    po::options_description command_options;

    // clang-format off
    command_options.add_options()
      (gInputStreamDirOptionName, po::value<std::string>()->required(), "Path saved market stream to test on")
      (gStrategyOptionName, po::value<std::string>()->required(), "Strategy name")
      (gOutputJsonOptionName, po::value<std::string>(), "Path where to save test result in json")
      (gNoTsJumpOptionName, po::bool_switch()->default_value(false), "Not use timestamps jumping");
    // clang-format on

    // Parse the options
    po::options_description all_options;
    all_options.add(command_options);
    po::store(po::parse_command_line(argc, argv, all_options), opts_map);
    po::notify(opts_map);
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  saved_stream_path_ = opts_map.at(gInputStreamDirOptionName).as<std::string>();
  disable_ts_jump_ = opts_map.at(gNoTsJumpOptionName).as<bool>();

  output_json_ = (opts_map.find(gOutputJsonOptionName) != opts_map.end());
  if (output_json_) {
    output_json_dir_ = opts_map.at(gOutputJsonOptionName).as<std::string>();
  }

  const auto strategy_str = opts_map.at(gStrategyOptionName).as<std::string>();
  if ("dummy" == strategy_str) {
    strategy_ = StrategyType::kDummy;
  } else {
    std::cerr << "Error: unknown strategy type" << std::endl;
    exit(EXIT_FAILURE);
  }

  spdlog::info("command pasing finished.");
}

void CommandStrategyTestHandler::Run() {
  spdlog::info("run start test command...");

  InitUnitStates();

  events::EventHub<events::message_queues::MarketStream> ms_event_hub;
  events::EventHub<events::message_queues::OrderBookStream> obs_event_hub;
  events::EventHub<events::message_queues::AnalyzerStream> as_event_hub;

  auto benchmark_data_collector = std::make_shared<analyzer::BenchmarkDataCollector>();

  // MS forwarder
  auto forwarder = std::make_shared<market_stream::SavedMarketStreamForwarder>(
      saved_stream_path_, ms_event_hub.dispatcher(), forwarder_state_);

  // MS reciever, OBS forwarder
  auto order_book_snap_provider = std::make_shared<analyzer::OrderBookSnapshotProvider>(
      ms_event_hub.CreateHandler(), obs_event_hub.dispatcher(), snapshot_provider_state_);

  // OBS reciever
  auto market_emulator = std::make_shared<analyzer::RealMarketEmulator>(
      obs_event_hub.CreateHandler(), real_market_emulator_state_);

  auto order_manager =
      std::make_shared<analyzer::OrderManager>(market_emulator, order_manager_state_);

  // MS reciever, AS forwarder
  std::shared_ptr<analyzer::ITradingStrategy> trading_strategy;
  trading_strategy = std::make_shared<analyzer::DummyTradingStrategy>(
      as_event_hub.dispatcher(), order_book_snap_provider);
  auto market_analyzer = std::make_shared<analyzer::MarketAnalyzer>(
      ms_event_hub.CreateHandler(), trading_strategy, analyzer_state_);

  // AS reciever
  auto order_plan_manager = std::make_shared<analyzer::OrderPlanManager>(
      as_event_hub.CreateHandler(), order_manager, order_plan_manager_state_,
      benchmark_data_collector);

  auto benchmark_orchestator =
      std::make_shared<analyzer::BenchmarkOrchestrator>(forwarder, !disable_ts_jump_);

  forwarder->Initialize();

  benchmark_orchestator->Go();

  ms_event_hub.Shutdown();
  obs_event_hub.Shutdown();
  as_event_hub.Shutdown();

  if (!output_json_) {
    std::cout << benchmark_data_collector->GenerateTotalReport() << std::endl;
  } else {
    const auto file_name =
        boost::filesystem::path(output_json_dir_) / gOutputJsonFileName;
    std::ofstream f(file_name.c_str(), std::ios::out);
    if (!f.is_open()) {
      std::cerr << "Failed to open the file." << std::endl;
    }
    f << benchmark_data_collector->GenerateTotalReportJson();
    f.close();
  }
}

void CommandStrategyTestHandler::InitUnitStates() {
  analyzer_state_ = std::make_shared<analyzer::AnalyzerUnitState>();
  order_plan_manager_state_ = std::make_shared<analyzer::OrderPlanManagerUnitState>();
  real_market_emulator_state_ = std::make_shared<analyzer::RealMarketEmulatorUnitState>();
  forwarder_state_ = std::make_shared<analyzer::SavedStreamForwarderUnitState>();
  snapshot_provider_state_ =
      std::make_shared<analyzer::OrderBookSnapshotProviderUnitState>();
  order_manager_state_ = std::make_shared<analyzer::OrderManagerUnitState>();
  transport_state_ = std::make_shared<analyzer::TransportUnitState>();

  events::SetTransportUnitState(transport_state_);
}

}  // namespace commands
