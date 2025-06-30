/**
 * @file command_streamload_handler.h
 * @brief Declaration of the CommandStreamLoadHandler interface.
 */

#ifndef INCLUDE_COMMAND_STREAMLOAD_HANDLER_H_
#define INCLUDE_COMMAND_STREAMLOAD_HANDLER_H_

#include <string>

#include "command_handler.h"

namespace commands {

/**
 * @class CommandStreamLoadHandler
 * @brief Command handler for streamload command.
 */
class CommandStreamLoadHandler : public CommandHandler {
 public:
  CommandStreamLoadHandler() = delete;
  CommandStreamLoadHandler(const CommandStreamLoadHandler &) = delete;
  CommandStreamLoadHandler(CommandStreamLoadHandler &&) = delete;
  CommandStreamLoadHandler &operator=(const CommandStreamLoadHandler &) = delete;
  CommandStreamLoadHandler &operator=(CommandStreamLoadHandler &&) = delete;

  CommandStreamLoadHandler(int argc, const char *argv[]);
  ~CommandStreamLoadHandler() = default;

  virtual void Run();

 private:
  std::string save_path_;
};

}  // namespace commands

#endif  // INCLUDE_COMMAND_STREAMLOAD_HANDLER_H_