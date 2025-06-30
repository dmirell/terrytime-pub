#include "market_stream/order_book_stream_forwarder.h"

#include <gtest/gtest.h>

#include <cctype>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <vector>

#include "market_stream/i_binapi_client.h"

namespace {
class FakeBinAPIClient : public market_stream::IBinAPIClient {
 public:
  static const int kOldDepthUpdatesCount = 3;
  static const int kRealDepthUpdatesCount = 4;

  FakeBinAPIClient(bool check_wrong_update_ids)
      : IBinAPIClient("BTCUSDT"), check_wrong_update_ids_(check_wrong_update_ids) {}

  void SubscribeToDepthStream(
      const DepthUpdateRawCallback& depth_book_update_cb) override {
    depth_update_cb_ = depth_book_update_cb;
  }
  void SubscribeToTradeStream(const TradeRawCallback& trade_cb) override {}
  void GetDepthAsync(const DepthRawCallback& depth_callback) override {
    depth_init_cb_ = depth_callback;
    depth_requested_.set_value(true);
  }
  void Run() override {
    run_finished_ = std::async(std::launch::async, [this]() {
      uint64_t previous_depth_u = 12342;
      for (int i = 0; i < kOldDepthUpdatesCount; i++) {
        binapi::ws::diff_depths_t diff_depth = BuildNextDiffDepth(previous_depth_u);
        depth_update_cb_(std::move(diff_depth));
      }
      auto depth_requested_future = depth_requested_.get_future();
      auto wait_result = depth_requested_future.wait_for(std::chrono::seconds(5));
      if (wait_result == std::future_status::ready && depth_requested_future.get()) {
        binapi::rest::depths_t depths;
        depths.lastUpdateId = previous_depth_u + 100;
        previous_depth_u = depths.lastUpdateId;
        depth_init_cb_(std::move(depths));
      }
      for (int i = 0; i < kRealDepthUpdatesCount; i++) {
        binapi::ws::diff_depths_t diff_depth = BuildNextDiffDepth(previous_depth_u);
        depth_update_cb_(std::move(diff_depth));
        if (check_wrong_update_ids_) {
          previous_depth_u = 0;
        }
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
  binapi::ws::diff_depths_t BuildNextDiffDepth(uint64_t& previous_depth_u) {
    binapi::ws::diff_depths_t diff_depth;
    diff_depth.U = previous_depth_u + 1;
    diff_depth.u = diff_depth.U + 5;
    previous_depth_u = diff_depth.u;
    return diff_depth;
  }

  bool check_wrong_update_ids_;
  std::future<void> run_finished_;
  std::promise<bool> depth_requested_;
  DepthUpdateRawCallback depth_update_cb_;
  DepthRawCallback depth_init_cb_;
};
}  // namespace

TEST(OrderBookStreamForwarder,
     GivenOrderBookStreamForwarder_WhenRun_ThenCallsOrderBookUpdates) {
  // Given
  int depths_calls_counter = 0;
  auto depth_update_callback =
      [&depths_calls_counter](const market_stream::types::OrderBook& update) {
        ++depths_calls_counter;
      };
  auto binapi_client = std::make_shared<FakeBinAPIClient>(false);
  market_stream::OrderBookStreamForwarder forwarder(binapi_client, depth_update_callback);

  // When
  auto forwarder_started = forwarder.StartAsync();
  binapi_client->Run();

  // Then
  ASSERT_NE(forwarder_started.wait_for(std::chrono::seconds(5)),
            std::future_status::timeout);
  ASSERT_TRUE(binapi_client->WaitForRunFinished());

  EXPECT_EQ(depths_calls_counter, FakeBinAPIClient::kRealDepthUpdatesCount + 1);
}

TEST(OrderBookStreamForwarder,
     GivenOrderBookStreamForwarder_WhenWrongUpdatesSequence_ThenAbort) {
  // Given
  int depths_calls_counter = 0;
  auto depth_update_callback =
      [&depths_calls_counter](const market_stream::types::OrderBook& update) {
        ++depths_calls_counter;
      };
  auto binapi_client = std::make_shared<FakeBinAPIClient>(true);
  market_stream::OrderBookStreamForwarder forwarder(binapi_client, depth_update_callback);
#ifndef NDEBUG
  EXPECT_DEATH(
      [&]() {
        // When
        auto forwarder_started = forwarder.StartAsync();
        binapi_client->Run();

        // Then
        forwarder_started.wait_for(std::chrono::seconds(5));
        binapi_client->WaitForRunFinished();
      }(),
      "");
#endif
}