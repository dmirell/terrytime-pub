#ifndef INCLUDE_ANALYZER_MARKET_STREAM_PRINTER_H_
#define INCLUDE_ANALYZER_MARKET_STREAM_PRINTER_H_

#include <condition_variable>
#include <memory>
#include <ostream>
#include <thread>
#include <vector>

#include "analyzer/observable_units.h"
#include "events/event_hub.h"
#include "market_stream/types/types.h"

namespace analyzer {

class OrderBookSnapshotProvider {
  using MQMarketStream = events::message_queues::MarketStream;
  using MQOrderBookStream = events::message_queues::OrderBookStream;
  using MQMSEventHubHandler = events::EventHub<MQMarketStream>::Handler;
  using MQOSEventHubDispatcher = events::EventHub<MQOrderBookStream>::Dispatcher;

 public:
  OrderBookSnapshotProvider() = delete;
  OrderBookSnapshotProvider(const OrderBookSnapshotProvider &) = delete;
  OrderBookSnapshotProvider(OrderBookSnapshotProvider &&) = delete;
  OrderBookSnapshotProvider &operator=(const OrderBookSnapshotProvider &) = delete;
  OrderBookSnapshotProvider &operator=(OrderBookSnapshotProvider &&) = delete;

  OrderBookSnapshotProvider(
      const std::weak_ptr<MQMSEventHubHandler> &event_handler,
      const std::weak_ptr<MQOSEventHubDispatcher> &dispatcher,
      const std::shared_ptr<OrderBookSnapshotProviderUnitState> &unit_state);
  ~OrderBookSnapshotProvider() = default;

  market_stream::types::OrderBook GetSnapshot(utils::Timestamp last_update_ts = 0);

 private:
  void OnMarketStreamEvent(MQMarketStream::Event event, const void *data);
  void OnOrderBookUpdateEvent(market_stream::types::OrderBook update);

  void PrepareItems(market_stream::types::OrderBook::Items *items,
                    bool is_items_order_increaseing);
  market_stream::types::OrderBook::Items UpdateOrderBookItems(
      const market_stream::types::OrderBook::Items &old_items,
      const market_stream::types::OrderBook::Items &update,
      bool is_items_order_increaseing);

  bool CheckItemsOrder(const market_stream::types::OrderBook::Items &items,
                       bool is_items_order_increaseing);
  bool IsCurrentSnapshotActual_locked() const;

  mutable std::mutex mutex_;
  std::condition_variable cv_;

  market_stream::types::OrderBook order_book_;

  std::weak_ptr<MQOSEventHubDispatcher> dispatcher_;
  std::shared_ptr<OrderBookSnapshotProviderUnitState> unit_state_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_MARKET_STREAM_PRINTER_H_