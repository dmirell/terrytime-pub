#include "analyzer/market_analyzer.h"

#include <spdlog/spdlog.h>

#include "analyzer/i_trading_strategy.h"
#include "analyzer/scoped_unit_state.h"

namespace analyzer {

MarketAnalyzer::MarketAnalyzer(const std::weak_ptr<EventHubHandler> &event_handler,
                               const std::shared_ptr<ITradingStrategy> &strategy,
                               const std::shared_ptr<AnalyzerUnitState> &unit_state)
    : unit_state_(unit_state), strategy_(strategy) {
  SUBSCRIBE_TO_EVENT(event_handler, &MarketAnalyzer::OnMarketStreamEvent);
}

void MarketAnalyzer::OnMarketStreamEvent(MQ::Event event, const void *data) {
  ScopedUnitState<ObservableUnits::UnitId::kAnalyzer> scoped_state(unit_state_);
  switch (event) {
    case MQ::Event::kNewTradeEvent:
      OnNewTrade(*static_cast<const market_stream::types::Trade *>(data));
      break;

    case MQ::Event::kOrderBookUpdateEvent:
      OnOrderBookUpdate(*static_cast<const market_stream::types::OrderBook *>(data));
      break;

    default:
      spdlog::error("Unknown MQ event");
      break;
  }
}

// All job must be done in sync maner here
void MarketAnalyzer::OnNewTrade(const market_stream::types::Trade &trade) {
  spdlog::debug("new trade recieved, r_ts: {}", trade.received_timestamp);
  strategy_->NewTrade(trade);
}

// All job must be done in sync maner here
void MarketAnalyzer::OnOrderBookUpdate(const market_stream::types::OrderBook &update) {
  spdlog::debug("new order book update recieved, r_ts: {}", update.received_timestamp);
  strategy_->OrderBookUpdate(update);
}
}  // namespace analyzer