
/**
 * @file command_streamsavehandler.h
 * @brief Declaration of the CommandStreamSaveHandler interface.
 */
#ifndef INCLUDE_COMMAND_STREAMSAVE_HANDLER_H_
#define INCLUDE_COMMAND_STREAMSAVE_HANDLER_H_

#include <chrono>
#include <string>

#include "command_handler.h"

namespace commands {

/**
 * @class CommandStreamSaveHandler
 * @brief Command handler for streamsave command.
 */
class CommandStreamSaveHandler : public CommandHandler {
 public:
  CommandStreamSaveHandler() = delete;
  CommandStreamSaveHandler(const CommandStreamSaveHandler &) = delete;
  CommandStreamSaveHandler(CommandStreamSaveHandler &&) = delete;
  CommandStreamSaveHandler &operator=(const CommandStreamSaveHandler &) = delete;
  CommandStreamSaveHandler &operator=(CommandStreamSaveHandler &&) = delete;

  CommandStreamSaveHandler(int argc, const char *argv[]);
  ~CommandStreamSaveHandler() = default;

  virtual void Run();

 private:
  bool print_stream_;
  std::string symbol_;
  std::string save_path_;
  std::chrono::seconds timer_;
};

}  // namespace commands

#endif  // INCLUDE_COMMAND_STREAMSAVE_HANDLER_H_