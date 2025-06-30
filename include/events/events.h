#ifndef INCLUDE_EVENTS_EVENTS_H_
#define INCLUDE_EVENTS_EVENTS_H_

#include "analyzer/types/types.h"
#include "market_stream/types/types.h"

#define MESSAGE_QUEUE(className) \
  enum class Event;              \
  template <Event id>            \
  struct EventTypeFromId;        \
  static constexpr const char* kClassName = className;

#define BIND_EVENT_TYPE(stream, event, bind_type) \
  template <>                                     \
  struct stream::EventTypeFromId<event> {         \
    using type = bind_type;                       \
  }

namespace events {
namespace message_queues {

// MarketStream
struct MarketStream {
  MESSAGE_QUEUE("MarketStream")

  enum class Event { kOrderBookUpdateEvent = 0, kNewTradeEvent, COUNT };
};

BIND_EVENT_TYPE(MarketStream, MarketStream::Event::kOrderBookUpdateEvent,
                market_stream::types::OrderBook);
BIND_EVENT_TYPE(MarketStream, MarketStream::Event::kNewTradeEvent,
                market_stream::types::Trade);

// OrderBookStream
struct OrderBookStream {
  MESSAGE_QUEUE("OrderBookStream")

  enum class Event { kNewSnapshotAvailable = 0, COUNT };
};

BIND_EVENT_TYPE(OrderBookStream, OrderBookStream::Event::kNewSnapshotAvailable,
                market_stream::types::OrderBook);

// AnalyzerStream
struct AnalyzerStream {
  MESSAGE_QUEUE("AnalyzerStream")

  enum class Event { kNewOrderPlan = 0, COUNT };
};

BIND_EVENT_TYPE(AnalyzerStream, AnalyzerStream::Event::kNewOrderPlan,
                analyzer::types::OrderPlanInfo);

}  // namespace message_queues
}  // namespace events

#endif  // INCLUDE_EVENTS_EVENTS_H_
