#include "commands/command_orderbook_test_handler.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>

#include "analyzer/benchmark_manager.h"
#include "analyzer/order_book_snapshot_provider.h"
#include "events/event_hub.h"
#include "market_stream/market_stream_forwarder.h"

namespace commands {

namespace {
const auto gSymbolOptionName = "symbol";
}  // namespace

CommandOrderBookTestHandler::CommandOrderBookTestHandler(int argc, const char* argv[]) {
  spdlog::info("command parsing...");
  po::variables_map opts_map;
  try {
    // clang-format off
    po::options_description command_options;
    command_options.add_options()
      (gSymbolOptionName, po::value<std::string>()->required(), "Symbol (e.g., BTCUSDT)");
    // clang-format on

    // Parse the options
    po::options_description all_options;
    all_options.add(command_options);
    po::store(po::parse_command_line(argc, argv, all_options), opts_map);
    po::notify(opts_map);
  } catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    exit(EXIT_FAILURE);
  }
  symbol_ = opts_map.at(gSymbolOptionName).as<std::string>();
  spdlog::info("command pasing finished.");
}

void CommandOrderBookTestHandler::Run() {
  spdlog::info("run command...");

  events::EventHub<events::message_queues::MarketStream> market_stream_event_hub;
  events::EventHub<events::message_queues::OrderBookStream> order_book_event_hub;

  std::shared_ptr<market_stream::MarketStreamForwarder> forwarder =
      std::make_shared<market_stream::MarketStreamForwarder>(
          symbol_, market_stream_event_hub.dispatcher());
  forwarder->Initialize();

  std::shared_ptr<analyzer::OrderBookSnapshotProvider> order_book_snapshot_provider =
      std::make_shared<analyzer::OrderBookSnapshotProvider>(
          market_stream_event_hub.CreateHandler(), order_book_event_hub.dispatcher(),
          nullptr);

  analyzer::BenchmarkManager benchmark_manager(symbol_, order_book_snapshot_provider);

  benchmark_manager.Run();

  forwarder->StartAsync();

  spdlog::info("command ran successfully.");
  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  market_stream_event_hub.Shutdown();
}

}  // namespace commands
