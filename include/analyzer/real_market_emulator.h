#ifndef INCLUDE_ANALYZER_REAL_MARKET_EMULATOR_H_
#define INCLUDE_ANALYZER_REAL_MARKET_EMULATOR_H_

#include <future>
#include <memory>
#include <thread>
#include <vector>

#include "analyzer/observable_units.h"
#include "events/event_hub.h"
#include "market_stream/types/types.h"
#include "types/types.h"

namespace analyzer {

class RealMarketEmulator {
  using MQ = events::message_queues::OrderBookStream;
  using EventHubHandler = events::EventHub<MQ>::Handler;

 public:
  RealMarketEmulator() = delete;
  RealMarketEmulator(const RealMarketEmulator &) = delete;
  RealMarketEmulator(RealMarketEmulator &&) = delete;
  RealMarketEmulator &operator=(const RealMarketEmulator &) = delete;
  RealMarketEmulator &operator=(RealMarketEmulator &&) = delete;

  RealMarketEmulator(const std::weak_ptr<EventHubHandler> &event_handler,
                     const std::shared_ptr<RealMarketEmulatorUnitState> &unit_state);
  virtual ~RealMarketEmulator() = default;

  virtual types::OrderResponse<std::future> PlaceOrder(
      const types::OrderInfo &order_info);
  virtual void CancelOrder(uint64_t order_id);

  std::size_t orders_count() const;

 private:
  struct Order {
    uint64_t id;
    types::OrderInfo info;
    types::OrderResponse<std::promise> responce;
  };
  void OnOrderBookStreamEvent(MQ::Event event, const void *data);

  types::OrderResponse<std::future> PlaceOrder_locked(const types::OrderInfo &order_info);
  void CancelOrder_locked(uint64_t order_id);
  std::size_t orders_count_locked() const;
  void OnOrderBookStreamEvent_locked(MQ::Event event, const void *data);

  std::vector<uint64_t> ProcessMarketTypeOrders();
  std::vector<uint64_t> CheckAndUpdateOrders(market_stream::types::DoubleType max_buy,
                                             market_stream::types::DoubleType min_sell);
  void RemoveFinishedOrders(const std::vector<uint64_t> &ids);
  std::vector<Order>::iterator FindOrderById(uint64_t order_id);
  bool CancelOrderById(uint64_t order_id);

  std::vector<Order> orders_;

  mutable std::mutex mutex_;
  market_stream::types::DoubleType last_max_buy_;
  market_stream::types::DoubleType last_min_sell_;
  std::shared_ptr<RealMarketEmulatorUnitState> unit_state_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_REAL_MARKET_EMULATOR_H_