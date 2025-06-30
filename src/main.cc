#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>

#include "commands/command_handler.h"
#include "commands/command_orderbook_test_handler.h"
#include "commands/command_strategy_test_handler.h"
#include "commands/command_strategy_test_online_handler.h"
#include "commands/command_stream_load_handler.h"
#include "commands/command_stream_save_handler.h"

int main(int argc, const char* argv[]) {
  spdlog::cfg::load_env_levels();
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%t] %v");

  std::string context;
  if (argc <= 1) {
    std::cerr << "You must specify the context.\n";
    return -1;
  }
  context = argv[1];

  std::unique_ptr<commands::CommandHandler> command_handler;
  if ("stream" == context) {
    if (argc <= 2) {
      std::cerr << "You must specify the command.\n";
      return -1;
    }
    std::string command = argv[2];
    if (command == "save") {
      command_handler = std::make_unique<commands::CommandStreamSaveHandler>(argc, argv);
    } else if (command == "load") {
      command_handler = std::make_unique<commands::CommandStreamLoadHandler>(argc, argv);
    } else {
      std::cerr << "Unknown command.\n";
      return -1;
    }
  } else if ("strategy" == context) {
    if (argc <= 2) {
      std::cerr << "You must specify the command.\n";
      return -1;
    }
    std::string command = argv[2];
    if (command == "test") {
      command_handler =
          std::make_unique<commands::CommandStrategyTestHandler>(argc, argv);
    } else if (command == "test-online") {
      command_handler =
          std::make_unique<commands::CommandStrategyTestOnlineHandler>(argc, argv);
    } else {
      std::cerr << "Unknown command.\n";
      return -1;
    }
  } else if ("orderbook" == context) {
    std::string command = argv[2];
    if (command == "test") {
      command_handler =
          std::make_unique<commands::CommandOrderBookTestHandler>(argc, argv);
    } else {
      std::cerr << "Unknown command.\n";
      return -1;
    }
  } else {
    std::cerr << "Unknown context.\n";
    return -1;
  }

  command_handler->Run();

  spdlog::shutdown();

  return 0;
}