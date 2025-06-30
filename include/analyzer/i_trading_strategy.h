#ifndef INCLUDE_ANALYZER_I_TRADING_STRATEGY_H_
#define INCLUDE_ANALYZER_I_TRADING_STRATEGY_H_

#include "events/event_hub.h"
#include "market_stream/types/types.h"

namespace analyzer {

class ITradingStrategy {
 protected:
  using MQAnalyzerStream = events::message_queues::AnalyzerStream;
  using MQAnalyzerStreamDispatcher = events::EventHub<MQAnalyzerStream>::Dispatcher;

 public:
  ITradingStrategy(const std::weak_ptr<MQAnalyzerStreamDispatcher> &dispatcher)
      : dispatcher_(dispatcher) {}
  virtual ~ITradingStrategy() = default;

  virtual void NewTrade(const market_stream::types::Trade &trade) = 0;
  virtual void OrderBookUpdate(const market_stream::types::OrderBook &update) = 0;

 protected:
  std::weak_ptr<MQAnalyzerStreamDispatcher> dispatcher_;
};
}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_I_TRADING_STRATEGY_H_