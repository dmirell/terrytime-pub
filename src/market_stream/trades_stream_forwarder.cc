#include "market_stream/trades_stream_forwarder.h"

#include <spdlog/spdlog.h>

#include "market_stream/binapi_client.h"

namespace market_stream {
TradeStreamForwarder::TradeStreamForwarder(
    const std::shared_ptr<IBinAPIClient>& binapi_client,
    const TradeCallback& trade_cb)
    : binapi_client_(binapi_client), trade_cb_(trade_cb) {
  spdlog::info("TradeStreamForwarder initialized");
  binapi_client_->SubscribeToTradeStream(std::bind(
      &TradeStreamForwarder::OnBinAPINewTrade, this, std::placeholders::_1));
}

std::future<void> TradeStreamForwarder::StartAsync() {
  spdlog::info("run trade stream forwarder...");
  return stream_started_promise_.get_future();
}

void TradeStreamForwarder::OnBinAPINewTrade(binapi::ws::trade_t&& trade) {
  if (!stream_started_) {
    stream_started_promise_.set_value();
    stream_started_ = true;
  }
  trade_cb_(std::move(trade));
}
}  // namespace market_stream