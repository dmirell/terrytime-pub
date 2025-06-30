#ifndef INCLUDE_UTILS_TESTS_HELPERS_FAKE_MARKET_STREAM_FORWARDER_H_
#define INCLUDE_UTILS_TESTS_HELPERS_FAKE_MARKET_STREAM_FORWARDER_H_

#include <ostream>
#include <vector>

#include "market_stream/market_stream_forwarder.h"

class FakeMarketStreamForwarder : public market_stream::MarketStreamForwarder {
 public:
  struct CombinedData {
    market_stream::types::OrderBook order_book;
    market_stream::types::Trade trade;
  };

  FakeMarketStreamForwarder(const std::weak_ptr<EventHubDispatcher>& event_dispatcher)
      : MarketStreamForwarder("BTCUSDT", event_dispatcher) {}

  void Initialize() override {
    CombinedData order_book_update_1, order_book_update_2;
    CombinedData trade_1, trade_2;

    // Initialize order book updates
    order_book_update_1.order_book.timestamp = 123456;
    order_book_update_1.order_book.received_timestamp = 123466;
    order_book_update_1.order_book.bids = {{121, 3.5}, {120, 22}};
    order_book_update_1.order_book.asks = {{122.2, 443.45}, {142.2, 43.5}, {155.2, 55}};
    order_book_update_2.order_book.timestamp = 124456;
    order_book_update_2.order_book.received_timestamp = 155466;
    order_book_update_2.order_book.bids = {
        {132.2, 43.5}, {123.2, 443.45}, {121, 0.0}, {120, 11}};
    order_book_update_2.order_book.asks = {{122.2, 0.0}, {134.2, 1}, {142.2, 12}};
    final_order_book_.timestamp = order_book_update_2.order_book.timestamp;
    final_order_book_.received_timestamp =
        order_book_update_2.order_book.received_timestamp;
    final_order_book_.bids = {{132.2, 43.5}, {123.2, 443.45}, {120, 11}};
    final_order_book_.asks = {{134.2, 1}, {142.2, 12}, {155.2, 55}};

    // Initialize trades updates
    trade_1.trade.event_timestamp = 12355;
    trade_1.trade.trade_timestamp = 3334;
    trade_1.trade.received_timestamp = 565464;
    trade_1.trade.price = 123.56;
    trade_1.trade.quantity = 22.4;
    trade_1.trade.is_buyer_maker = true;
    trade_2.trade.event_timestamp = 13232355;
    trade_2.trade.trade_timestamp = 333334;
    trade_2.trade.received_timestamp = 62345667;
    trade_2.trade.price = 423.56;
    trade_2.trade.quantity = 252.45;
    trade_2.trade.is_buyer_maker = false;

    fake_stream_.push_back(
        {market_stream::types::MarketDataType::ORDER_BOOK, order_book_update_1});
    fake_stream_.push_back({market_stream::types::MarketDataType::TRADE, trade_1});
    fake_stream_.push_back(
        {market_stream::types::MarketDataType::ORDER_BOOK, order_book_update_2});
    fake_stream_.push_back({market_stream::types::MarketDataType::TRADE, trade_2});
    fake_stream_.push_back({market_stream::types::MarketDataType::TRADE, trade_1});
    fake_stream_.push_back({market_stream::types::MarketDataType::TRADE, trade_2});
  }

  void Start() {
    for (const auto& it : fake_stream_) {
      auto value = it.second;
      auto event_dispatcher_locked = event_dispatcher_.lock();
      if (event_dispatcher_locked == nullptr) {
        spdlog::error("Event dispatcher now available");
        continue;
      }
      switch (it.first) {
        case market_stream::types::MarketDataType::ORDER_BOOK:
          event_dispatcher_locked->DispatchEvent<MQ::Event::kOrderBookUpdateEvent>(
              value.order_book);
          break;

        case market_stream::types::MarketDataType::TRADE:
        default:
          event_dispatcher_locked->DispatchEvent<MQ::Event::kNewTradeEvent>(value.trade);
          break;
      }
    }
  }

  void VerifyRecievedStream(
      const std::vector<std::pair<market_stream::types::MarketDataType, CombinedData>>&
          recieved_stream) const {
    ASSERT_EQ(recieved_stream.size(), fake_stream_.size());
    for (int i = 0; i < fake_stream_.size(); i++) {
      EXPECT_EQ(fake_stream_[i].first, recieved_stream[i].first);
      switch (fake_stream_[i].first) {
        case market_stream::types::MarketDataType::ORDER_BOOK:
          EXPECT_EQ(recieved_stream[i].second.order_book,
                    fake_stream_[i].second.order_book);
          break;

        case market_stream::types::MarketDataType::TRADE:
        default:
          EXPECT_EQ(recieved_stream[i].second.trade, fake_stream_[i].second.trade);
          break;
      }
    }
  }

  void VerifyOrderBookHandle(const market_stream::types::OrderBook& order_book) const {
    EXPECT_EQ(final_order_book_, order_book);
  }

  void PrintStream(std::ostream& os) const {
    for (const auto& it : fake_stream_) {
      switch (it.first) {
        case market_stream::types::MarketDataType::ORDER_BOOK:
          os << it.second.order_book;
          break;

        case market_stream::types::MarketDataType::TRADE:
        default:
          os << it.second.trade;
          break;
      }
    }
  }

  std::vector<std::pair<market_stream::types::MarketDataType, CombinedData>> fake_stream_;

 private:
  market_stream::types::OrderBook final_order_book_;
};

#endif  // INCLUDE_UTILS_TESTS_HELPERS_FAKE_MARKET_STREAM_FORWARDER_H_
