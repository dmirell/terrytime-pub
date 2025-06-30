#ifndef INCLUDE_ANALYZER_DUMMY_TRADING_STRATEGY_H_
#define INCLUDE_ANALYZER_DUMMY_TRADING_STRATEGY_H_

#include "analyzer/i_trading_strategy.h"
#include "utils/time/types.h"

namespace analyzer {

class OrderBookSnapshotProvider;

class DummyTradingStrategy : public ITradingStrategy {
 public:
  DummyTradingStrategy(
      const std::weak_ptr<MQAnalyzerStreamDispatcher> &dispatcher,
      const std::shared_ptr<OrderBookSnapshotProvider> &order_book_snap_provider);

  // ITradingStrategy
  void NewTrade(const market_stream::types::Trade &trade) override;
  void OrderBookUpdate(const market_stream::types::OrderBook &update) override;

 private:
  void DummyMethod(utils::Timestamp received_timestamp);

  utils::Timestamp last_sent_r_ts_{0};
  std::shared_ptr<OrderBookSnapshotProvider> order_book_snap_provider_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_DUMMY_TRADING_STRATEGY_H_