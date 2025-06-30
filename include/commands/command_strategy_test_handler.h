
/**
 * @file command_streamsavehandler.h
 * @brief Declaration of the CommandStrategyTestHandler interface.
 */
#ifndef INCLUDE_COMMAND_TEST_STRATEGY_HANDLER_H_
#define INCLUDE_COMMAND_TEST_STRATEGY_HANDLER_H_

#include <chrono>
#include <string>

#include "analyzer/observable_units.h"
#include "command_handler.h"
#include "commands/types.h"

namespace commands {

/**
 * @class CommandStrategyTestHandler
 * @brief Command handler for streamsave command.
 */
class CommandStrategyTestHandler : public CommandHandler {
 public:
  CommandStrategyTestHandler() = delete;
  CommandStrategyTestHandler(const CommandStrategyTestHandler &) = delete;
  CommandStrategyTestHandler(CommandStrategyTestHandler &&) = delete;
  CommandStrategyTestHandler &operator=(const CommandStrategyTestHandler &) = delete;
  CommandStrategyTestHandler &operator=(CommandStrategyTestHandler &&) = delete;

  CommandStrategyTestHandler(int argc, const char *argv[]);
  ~CommandStrategyTestHandler() = default;

  virtual void Run();

 private:
  void InitUnitStates();

  std::string saved_stream_path_;
  std::string output_json_dir_;
  bool disable_ts_jump_;
  bool output_json_;
  StrategyType strategy_;

  std::shared_ptr<analyzer::AnalyzerUnitState> analyzer_state_;
  std::shared_ptr<analyzer::OrderPlanManagerUnitState> order_plan_manager_state_;
  std::shared_ptr<analyzer::RealMarketEmulatorUnitState> real_market_emulator_state_;
  std::shared_ptr<analyzer::SavedStreamForwarderUnitState> forwarder_state_;
  std::shared_ptr<analyzer::OrderBookSnapshotProviderUnitState> snapshot_provider_state_;
  std::shared_ptr<analyzer::OrderManagerUnitState> order_manager_state_;
  std::shared_ptr<analyzer::TransportUnitState> transport_state_;
};

}  // namespace commands

#endif  // INCLUDE_COMMAND_TEST_STRATEGY_HANDLER_H_