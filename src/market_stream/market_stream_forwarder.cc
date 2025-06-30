#include "market_stream/market_stream_forwarder.h"

#include <spdlog/spdlog.h>

#include "market_stream/binapi_client.h"
#include "utils/time/global_clock.h"

namespace market_stream {

MarketStreamForwarder::MarketStreamForwarder(
    const std::string& symbol, const std::weak_ptr<EventHubDispatcher>& event_dispatcher)
    : symbol_(symbol), event_dispatcher_(event_dispatcher) {}

void MarketStreamForwarder::Initialize() {
  spdlog::info("MarketStreamForwarder initializing for {} pair", symbol_);

  binapi_client_ = std::make_shared<BinAPIClient>(symbol_);
  trade_stream_ = std::make_unique<TradeStreamForwarder>(
      binapi_client_,
      std::bind(&MarketStreamForwarder::OnTradeReceived, this, std::placeholders::_1));
  order_book_stream_ = std::make_unique<OrderBookStreamForwarder>(
      binapi_client_,
      std::bind(&MarketStreamForwarder::OnOrderBookUpdate, this, std::placeholders::_1));
  spdlog::info("run binapi client");
  binapi_client_->Run();
  spdlog::info("MarketStreamForwarder initialized");
}

void MarketStreamForwarder::StartAsync() {
  spdlog::info("run market stream forwarder...");
}

void MarketStreamForwarder::OnTradeReceived(types::Trade&& trade) {
  if (order_book_stream_started_) {
    trade.received_timestamp = utils::GlobalClock::Instance().Now();
    auto event_dispatcher_locked = event_dispatcher_.lock();
    if (event_dispatcher_locked) {
      event_dispatcher_locked->DispatchEvent<MQ::Event::kNewTradeEvent>(trade);
    }
  }
}

void MarketStreamForwarder::OnOrderBookUpdate(types::OrderBook&& update) {
  if (!order_book_stream_started_.load()) {
    order_book_stream_started_.store(true);
    spdlog::info("market stream started");
  }
  update.received_timestamp = utils::GlobalClock::Instance().Now();
  auto event_dispatcher_locked = event_dispatcher_.lock();
  if (event_dispatcher_locked) {
    event_dispatcher_locked->DispatchEvent<MQ::Event::kOrderBookUpdateEvent>(update);
  }
}

}  // namespace market_stream