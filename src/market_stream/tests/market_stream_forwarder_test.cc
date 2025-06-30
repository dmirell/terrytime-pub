#include "market_stream/market_stream_forwarder.h"

#include <gtest/gtest.h>

#include <functional>
#include <future>
#include <memory>
#include <thread>

#include "events/event_hub.h"
#include "market_stream/i_binapi_client.h"
#include "market_stream/order_book_stream_forwarder.h"
#include "market_stream/trades_stream_forwarder.h"

namespace {
class FakeBinAPIClient : public market_stream::IBinAPIClient {
 public:
  FakeBinAPIClient() : IBinAPIClient("BTCUSDT") {}
  void SubscribeToDepthStream(
      const DepthUpdateRawCallback& depth_book_update_cb) override {}
  void SubscribeToTradeStream(const TradeRawCallback& trade_cb) override {}
  void GetDepthAsync(const DepthRawCallback& depth_callback) override {}
  void Run() override {}
};

class FakeOrderBookStreamForwarder : public market_stream::OrderBookStreamForwarder {
 public:
  FakeOrderBookStreamForwarder(
      const std::shared_ptr<market_stream::IBinAPIClient>& binapi_client,
      const OrderBookUpdateCallback& order_book_update_cb)
      : OrderBookStreamForwarder(binapi_client, order_book_update_cb) {}
  void EmulateUpdateOrderBook() {
    order_book_update_cb_(market_stream::types::OrderBook{});
    if (!stream_started_) {
      stream_started_ = true;
      stream_started_promise_.set_value();
    }
  }

 private:
  bool stream_started_{false};
};

class FakeTradeStreamForwarder : public market_stream::TradeStreamForwarder {
 public:
  FakeTradeStreamForwarder(
      const std::shared_ptr<market_stream::IBinAPIClient>& binapi_client,
      const TradeCallback& trade_cb)
      : TradeStreamForwarder(binapi_client, trade_cb) {}
  void EmulateNewTrade() {
    trade_cb_(market_stream::types::Trade{});
    if (!stream_started_) {
      stream_started_ = true;
      stream_started_promise_.set_value();
    }
  }

 private:
  bool stream_started_{false};
};

class FakeMarketStreamForwarder : public market_stream::MarketStreamForwarder {
 public:
  FakeMarketStreamForwarder(const std::weak_ptr<EventHubDispatcher>& event_dispatcher)
      : MarketStreamForwarder("BTCUSDT", event_dispatcher) {}

  void Initialize() override {
    auto binapi_client = std::make_shared<FakeBinAPIClient>();
    trade_stream_ = std::make_unique<FakeTradeStreamForwarder>(
        binapi_client, std::bind(&FakeMarketStreamForwarder::OnTradeReceived, this,
                                 std::placeholders::_1));
    order_book_stream_ = std::make_unique<FakeOrderBookStreamForwarder>(
        binapi_client, std::bind(&FakeMarketStreamForwarder::OnOrderBookUpdate, this,
                                 std::placeholders::_1));
  }

  std::future<void> EmulateBinAPIRun() {
    return std::async(std::launch::async, [this]() {
      std::chrono::seconds(1);
      for (int i = 0; i < kDummyNewTradesCount; i++) {
        EmulateNewTrade();
      }
      for (int i = 0; i < kRealEventsCount; i++) {
        EmulateUpdateOrderBook();
        EmulateNewTrade();
      }
    });
  }

  static const int kDummyNewTradesCount;
  static const int kRealEventsCount;

 private:
  void EmulateUpdateOrderBook() {
    const auto fake_order_book_stream =
        dynamic_cast<FakeOrderBookStreamForwarder*>(order_book_stream_.get());
    if (fake_order_book_stream) {
      fake_order_book_stream->EmulateUpdateOrderBook();
    }
  }
  void EmulateNewTrade() {
    const auto fake_trade_stream =
        dynamic_cast<FakeTradeStreamForwarder*>(trade_stream_.get());
    if (fake_trade_stream) {
      fake_trade_stream->EmulateNewTrade();
    }
  }
};

const int FakeMarketStreamForwarder::kDummyNewTradesCount = 3;
const int FakeMarketStreamForwarder::kRealEventsCount = 5;

}  // namespace

TEST(MarketStreamForwarder, GivenMarketStreamForwarder_WhenNewEvents_ThenCallbacksSent) {
  using MQ = events::message_queues::MarketStream;
  // Given
  int order_book_received_count = 0;
  int trades_received_count = 0;
  events::EventHub<MQ> event_hub;

  std::shared_ptr<FakeMarketStreamForwarder> fake_forwarder =
      std::make_shared<FakeMarketStreamForwarder>(event_hub.dispatcher());
  fake_forwarder->Initialize();
  event_hub.CreateHandler().lock()->Subscribe([&](MQ::Event event, const void* data) {
    switch (event) {
      case MQ::Event::kNewTradeEvent:
        trades_received_count++;
        break;
      case MQ::Event::kOrderBookUpdateEvent:
        order_book_received_count++;
        break;
    }
  });

  // When
  fake_forwarder->StartAsync();
  auto future = fake_forwarder->EmulateBinAPIRun();  // REDO it
  ASSERT_EQ(future.wait_for(std::chrono::seconds(5)), std::future_status::ready);
  event_hub.Shutdown();

  // Then
  EXPECT_EQ(order_book_received_count, FakeMarketStreamForwarder::kRealEventsCount);
  EXPECT_EQ(trades_received_count, FakeMarketStreamForwarder::kRealEventsCount);
}