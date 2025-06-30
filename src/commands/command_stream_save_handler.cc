#include "commands/command_stream_save_handler.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "events/event_hub.h"
#include "market_stream/market_stream_forwarder.h"
#include "market_stream/market_stream_printer.h"
#include "market_stream/market_stream_saver.h"
#include "utils/helpers.h"

namespace commands {

namespace {
const auto gSymbolOptionName = "symbol";
const auto gDurationOptionName = "timer";
const auto gPrintStreamOptionName = "print-stream";
const auto gOutputDirOptionName = "output-dir";
}  // namespace

CommandStreamSaveHandler::CommandStreamSaveHandler(int argc, const char* argv[]) {
  spdlog::info("command parsing...");
  po::variables_map opts_map;
  try {
    // clang-format off
    po::options_description command_options;
    command_options.add_options()
      (gSymbolOptionName, po::value<std::string>()->required(), "Symbol (e.g., BTCUSDT)")
      (gOutputDirOptionName, po::value<std::string>()->required(), "Path to the stream save dir")
      (gDurationOptionName, po::value<int>()->required(), "Timer duration value in seconds")
      (gPrintStreamOptionName, po::bool_switch()->default_value(false), "Print recording stream");
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
  save_path_ = opts_map.at(gOutputDirOptionName).as<std::string>();
  timer_ = std::chrono::seconds(opts_map.at(gDurationOptionName).as<int>());
  print_stream_ = opts_map.at(gPrintStreamOptionName).as<bool>();
  spdlog::info("command pasing finished.");
}

void CommandStreamSaveHandler::Run() {
  spdlog::info("run stream save command...");

  events::EventHub<events::message_queues::MarketStream> event_hub;

  std::shared_ptr<market_stream::MarketStreamForwarder> forwarder =
      std::make_shared<market_stream::MarketStreamForwarder>(symbol_,
                                                             event_hub.dispatcher());
  forwarder->Initialize();

  market_stream::MarketStreamSaver stream_saver(event_hub.CreateHandler(), save_path_);
  std::shared_ptr<market_stream::MarketStreamPrinter> stream_printer;

  if (print_stream_) {
    stream_printer = std::make_shared<market_stream::MarketStreamPrinter>(
        event_hub.CreateHandler(), std::cout);
  }
  forwarder->StartAsync();

  spdlog::info("stream save run successfully.");
  std::this_thread::sleep_for(std::chrono::seconds(timer_));
  spdlog::info("timer finished");

  event_hub.Shutdown();
}

}  // namespace commands
