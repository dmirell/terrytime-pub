#ifndef INCLUDE_MARKET_STREAM_MARKET_STREAM_FORWARDER_H_
#define INCLUDE_MARKET_STREAM_MARKET_STREAM_FORWARDER_H_

#include <atomic>
#include <cctype>
#include <memory>
#include <string>

#include "events/event_hub.h"
#include "order_book_stream_forwarder.h"
#include "trades_stream_forwarder.h"
#include "types/types.h"

namespace market_stream {

class BinAPIClient;

class MarketStreamForwarder {
 protected:
  using MQ = events::message_queues::MarketStream;
  using EventHubDispatcher = events::EventHub<MQ>::Dispatcher;

 public:
  MarketStreamForwarder() = delete;
  MarketStreamForwarder(const MarketStreamForwarder &) = delete;
  MarketStreamForwarder(MarketStreamForwarder &&) = delete;
  MarketStreamForwarder &operator=(const MarketStreamForwarder &) = delete;
  MarketStreamForwarder &operator=(MarketStreamForwarder &&) = delete;

  MarketStreamForwarder(const std::string &symbol,
                        const std::weak_ptr<EventHubDispatcher> &event_dispatcher);
  ~MarketStreamForwarder() = default;

  virtual void Initialize();
  virtual void StartAsync();

 protected:
  void OnTradeReceived(types::Trade &&trade);
  void OnOrderBookUpdate(types::OrderBook &&update);

  std::shared_ptr<BinAPIClient> binapi_client_;
  std::unique_ptr<TradeStreamForwarder> trade_stream_;
  std::unique_ptr<OrderBookStreamForwarder> order_book_stream_;
  std::weak_ptr<EventHubDispatcher> event_dispatcher_;

 private:
  std::string symbol_;
  std::atomic<bool> order_book_stream_started_{false};
};

}  // namespace market_stream

#endif  // INCLUDE_MARKET_STREAM_MARKET_STREAM_FORWARDER_H_