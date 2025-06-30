/**
 * @file command_handler.h
 * @brief Declaration of the CommandHandler interface.
 */

#ifndef INCLUDE_COMMAND_HANDLER_H_
#define INCLUDE_COMMAND_HANDLER_H_

#include <boost/program_options.hpp>

namespace commands {

namespace po = boost::program_options;

/**
 * @class CommandHandler
 * @brief Interface for application commands handlers.
 */
class CommandHandler {
 public:
  /**
   * @brief Run command method.
   */
  virtual void Run() = 0;

  virtual ~CommandHandler() {}
};

};  // namespace commands

#endif  // INCLUDE_COMMAND_HANDLER_H_