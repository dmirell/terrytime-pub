#include "market_stream/trades_stream_forwarder.h"

#include <gtest/gtest.h>

#include <functional>
#include <future>
#include <memory>

#include "market_stream/i_binapi_client.h"

namespace {
class FakeBinAPIClient : public market_stream::IBinAPIClient {
 public:
  static const int kNewTradesCount;

  FakeBinAPIClient() : IBinAPIClient("BTCUSDT") {}

  void SubscribeToDepthStream(
      const DepthUpdateRawCallback& depth_book_update_cb) override {}
  void SubscribeToTradeStream(const TradeRawCallback& trade_cb) override {
    trade_cb_ = trade_cb;
  }
  void GetDepthAsync(const DepthRawCallback& depth_callback) override {}
  void Run() override {
    run_finished_ = std::async(std::launch::async, [this]() {
      for (int i = 0; i < kNewTradesCount; i++) {
        binapi::ws::trade_t trade;
        trade_cb_(binapi::ws::trade_t{});
      }
    });
  }

  bool WaitForRunFinished() {
    if (run_finished_.wait_for(std::chrono::seconds(15)) == std::future_status::ready) {
      run_finished_.get();
      return true;
    }
    return false;
  }

 private:
  std::future<void> run_finished_;
  TradeRawCallback trade_cb_;
};

const int FakeBinAPIClient::kNewTradesCount = 7;

}  // namespace

TEST(TradesStreamForwarder, GivenTradesStreamForwarder_WhenRun_ThenRecieveNewTrades) {
  // Given
  int received_trades_counter = 0;
  auto trade_callback =
      [&received_trades_counter](const market_stream::types::Trade& update) {
        ++received_trades_counter;
      };
  auto binapi_client = std::make_shared<FakeBinAPIClient>();
  market_stream::TradeStreamForwarder forwarder(binapi_client, trade_callback);

  // When
  auto forwarder_started = forwarder.StartAsync();
  binapi_client->Run();

  // Then
  ASSERT_NE(forwarder_started.wait_for(std::chrono::seconds(5)),
            std::future_status::timeout);
  ASSERT_TRUE(binapi_client->WaitForRunFinished());

  EXPECT_EQ(received_trades_counter, FakeBinAPIClient::kNewTradesCount);
}