
#ifndef INCLUDE_COMMAND_TEST_ORDERBOOK_HANDLER_H_
#define INCLUDE_COMMAND_TEST_ORDERBOOK_HANDLER_H_

#include <string>

#include "command_handler.h"

namespace commands {

class CommandOrderBookTestHandler : public CommandHandler {
 public:
  CommandOrderBookTestHandler() = delete;
  CommandOrderBookTestHandler(const CommandOrderBookTestHandler &) = delete;
  CommandOrderBookTestHandler(CommandOrderBookTestHandler &&) = delete;
  CommandOrderBookTestHandler &operator=(const CommandOrderBookTestHandler &) = delete;
  CommandOrderBookTestHandler &operator=(CommandOrderBookTestHandler &&) = delete;

  CommandOrderBookTestHandler(int argc, const char *argv[]);
  ~CommandOrderBookTestHandler() = default;

  virtual void Run();

 private:
  std::string symbol_;
};

}  // namespace commands

#endif  // INCLUDE_COMMAND_TEST_ORDERBOOK_HANDLER_H_