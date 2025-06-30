#ifndef INCLUDE_ANALYZER_MARKET_ANALYZER_H_
#define INCLUDE_ANALYZER_MARKET_ANALYZER_H_

#include <memory>

#include "analyzer/observable_units.h"
#include "events/event_hub.h"
#include "market_stream/types/types.h"

namespace analyzer {

class ITradingStrategy;
class OrderBookSnapshotProvider;

class MarketAnalyzer {
  using MQ = events::message_queues::MarketStream;
  using EventHubHandler = events::EventHub<MQ>::Handler;

 public:
  MarketAnalyzer() = delete;
  MarketAnalyzer(const MarketAnalyzer &) = delete;
  MarketAnalyzer(MarketAnalyzer &&) = delete;
  MarketAnalyzer &operator=(const MarketAnalyzer &) = delete;
  MarketAnalyzer &operator=(MarketAnalyzer &&) = delete;

  MarketAnalyzer(const std::weak_ptr<EventHubHandler> &event_handler,
                 const std::shared_ptr<ITradingStrategy> &strategy,
                 const std::shared_ptr<AnalyzerUnitState> &unit_state);
  ~MarketAnalyzer() = default;

 private:
  void OnMarketStreamEvent(MQ::Event event, const void *data);
  void OnNewTrade(const market_stream::types::Trade &trade);
  void OnOrderBookUpdate(const market_stream::types::OrderBook &update);

  std::shared_ptr<AnalyzerUnitState> unit_state_;
  std::shared_ptr<ITradingStrategy> strategy_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_MARKET_ANALYZER_H_