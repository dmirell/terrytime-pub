
/**
 * @file command_streamsavehandler.h
 * @brief Declaration of the CommandStrategyTestOnlineHandler interface.
 */
#ifndef INCLUDE_COMMAND_TEST_ONLINE_STRATEGY_HANDLER_H_
#define INCLUDE_COMMAND_TEST_ONLINE_STRATEGY_HANDLER_H_

#include <chrono>
#include <string>

#include "analyzer/observable_units.h"
#include "command_handler.h"
#include "commands/types.h"

namespace commands {

/**
 * @class CommandStrategyTestOnlineHandler
 * @brief Command handler for streamsave command.
 */
class CommandStrategyTestOnlineHandler : public CommandHandler {
 public:
  CommandStrategyTestOnlineHandler() = delete;
  CommandStrategyTestOnlineHandler(const CommandStrategyTestOnlineHandler &) = delete;
  CommandStrategyTestOnlineHandler(CommandStrategyTestOnlineHandler &&) = delete;
  CommandStrategyTestOnlineHandler &operator=(const CommandStrategyTestOnlineHandler &) =
      delete;
  CommandStrategyTestOnlineHandler &operator=(CommandStrategyTestOnlineHandler &&) =
      delete;

  CommandStrategyTestOnlineHandler(int argc, const char *argv[]);
  ~CommandStrategyTestOnlineHandler() = default;

  virtual void Run();

 private:
  void InitUnitStates();

  std::string symbol_;
  std::string output_dir_;
  StrategyType strategy_;
  std::chrono::seconds duration_;

  std::shared_ptr<analyzer::AnalyzerUnitState> analyzer_state_;
  std::shared_ptr<analyzer::OrderPlanManagerUnitState> order_plan_manager_state_;
  std::shared_ptr<analyzer::RealMarketEmulatorUnitState> real_market_emulator_state_;
  std::shared_ptr<analyzer::SavedStreamForwarderUnitState> forwarder_state_;
  std::shared_ptr<analyzer::OrderBookSnapshotProviderUnitState> snapshot_provider_state_;
  std::shared_ptr<analyzer::OrderManagerUnitState> order_manager_state_;
  std::shared_ptr<analyzer::TransportUnitState> transport_state_;
};

}  // namespace commands

#endif  // INCLUDE_COMMAND_TEST_ONLINE_STRATEGY_HANDLER_H_