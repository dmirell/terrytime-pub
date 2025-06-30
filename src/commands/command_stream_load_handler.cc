#include "commands/command_stream_load_handler.h"

#include <spdlog/spdlog.h>

#include <iostream>

#include "events/event_hub.h"
#include "market_stream/market_stream_printer.h"
#include "market_stream/saved_market_stream_forwarder.h"

namespace commands {

namespace {
const auto gSavedStreamDirOptionName = "stream-dir";
}

CommandStreamLoadHandler::CommandStreamLoadHandler(int argc, const char* argv[]) {
  spdlog::info("command parsing...");
  po::variables_map opts_map;
  try {
    // Define options specific to the "streamsaver" command
    po::options_description command_options;

    // clang-format off
    command_options.add_options()
      (gSavedStreamDirOptionName, po::value<std::string>()->required(), "Path to the recorded market stream file dir");
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

  save_path_ = opts_map.at(gSavedStreamDirOptionName).as<std::string>();
  spdlog::info("command parsing finished.");
}

void CommandStreamLoadHandler::Run() {
  spdlog::info("run stream load command...");

  events::EventHub<events::message_queues::MarketStream> event_hub;
  std::shared_ptr<market_stream::SavedMarketStreamForwarder> forwarder =
      std::make_shared<market_stream::SavedMarketStreamForwarder>(save_path_,
                                                                  event_hub.dispatcher());
  forwarder->Initialize();

  market_stream::MarketStreamPrinter printer(event_hub.CreateHandler(), std::cout);

  while (forwarder->ReadNext()) {
    forwarder->ForwardNext();
  }

  event_hub.Shutdown();
}

}  // namespace commands
