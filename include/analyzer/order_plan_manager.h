#ifndef INCLUDE_ANALYZER_ORDER_PLAN_MANAGER_H_
#define INCLUDE_ANALYZER_ORDER_PLAN_MANAGER_H_

#include <condition_variable>
#include <memory>
#include <thread>

#include "analyzer/observable_units.h"
#include "events/event_hub.h"
#include "market_stream/types/types.h"
#include "types/types.h"
#include "utils/time/types.h"

namespace analyzer {

class OrderManager;
class BenchmarkDataCollector;

class OrderPlanManager {
  using MQ = events::message_queues::AnalyzerStream;
  using EventHubHandler = events::EventHub<MQ>::Handler;

 public:
  OrderPlanManager() = delete;
  OrderPlanManager(const OrderPlanManager &) = delete;
  OrderPlanManager(OrderPlanManager &&) = delete;
  OrderPlanManager &operator=(const OrderPlanManager &) = delete;
  OrderPlanManager &operator=(OrderPlanManager &&) = delete;

  OrderPlanManager(
      const std::weak_ptr<EventHubHandler> &event_handler,
      const std::shared_ptr<OrderManager> &order_manager,
      const std::shared_ptr<OrderPlanManagerUnitState> &unit_state,
      const std::shared_ptr<BenchmarkDataCollector> &benchmark_data_collector);
  ~OrderPlanManager();

 private:
  enum class OrderPlanStage {
    kNotStarted = 0,
    kBuyOrderPlaced,
    kBuyOrderTimeout,
    kSellOrderPlaced,
    kSellOrderTimeout,
    COUNT
  };

  void OnOrderPlanStreamEvent(MQ::Event event, const void *data);
  void ProcessOrderPlan(types::OrderPlanInfo);
  bool ExecOrder(const types::OrderInfo &order, utils::Timestamp expiration_ts,
                 types::OrderResult *result);
  void SendReport(market_stream::types::DoubleType buy_price,
                  market_stream::types::DoubleType sold_price,
                  utils::Timestamp start_time_ms, utils::TimestampPrecision duration_ms,
                  types::OrderPlanReport::Status status);
  void OrderPlansProcessThread();

  types::OrderPlanInfo current_order_info_;
  std::shared_ptr<OrderManager> order_manager_;
  std::shared_ptr<OrderPlanManagerUnitState> unit_state_;
  std::shared_ptr<BenchmarkDataCollector> benchmark_data_collector_;

  std::shared_ptr<types::OrderPlanInfo> order_plan_info_;
  mutable std::mutex mutex_;
  std::condition_variable cv_;
  std::thread order_plans_processing_thread_;
  std::atomic<bool> is_stopped_{false};
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_ORDER_PLAN_MANAGER_H_