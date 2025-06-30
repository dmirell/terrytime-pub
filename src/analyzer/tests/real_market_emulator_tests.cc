#include <gmock/gmock.h>

#include <chrono>
#include <future>

#include "analyzer/benchmark_orchestrator.h"
#include "analyzer/real_market_emulator.h"
#include "analyzer/types/types.h"
#include "events/event_hub.h"
#include "market_stream/types/types.h"
#include "utils/tests/helpers/unit_state_mock.h"

namespace analyzer {

namespace {
class RealMarketEmulatorFixture : public ::testing::Test {
  using MQ = events::message_queues::OrderBookStream;
  using EventHubHandler = events::EventHub<MQ>::Handler;

 public:
  RealMarketEmulatorFixture() {
    unit_state_ =
        std::make_shared<MockUnitState<ObservableUnits::UnitId::kRealMarketEmulator>>();
    real_market_emulator_ =
        std::make_shared<RealMarketEmulator>(event_hub_.CreateHandler(), unit_state_);
    order_book1_.timestamp = 123456789;
    order_book1_.bids = {{132.2, 43.5}, {123.2, 443.45}, {120, 11}};
    order_book1_.asks = {{134.2, 1}, {142.2, 12}, {155.2, 55}};
    order_book2_.timestamp = 123476789;
    order_book2_.bids = {{133.2, 43.5}, {123.2, 443.45}, {120, 11}};
    order_book2_.asks = {{133.6, 1}, {142.2, 12}, {155.2, 55}};

    order_1.first.side = types::OrderInfo::Side::BUY;
    order_1.first.type = types::OrderInfo::Type::LIMIT;
    order_1.first.price = 133.2;
    order_1.first.quantity = 23;
    order_2.first.side = types::OrderInfo::Side::SELL;
    order_2.first.type = types::OrderInfo::Type::LIMIT;
    order_2.first.price = 134.1;
    order_2.first.quantity = 23;
    order_3.first.side = types::OrderInfo::Side::BUY;
    order_3.first.type = types::OrderInfo::Type::LIMIT;
    order_3.first.price = 120.1;
    order_3.first.quantity = 23;
    order_4.first.side = types::OrderInfo::Side::SELL;
    order_4.first.type = types::OrderInfo::Type::LIMIT;
    order_4.first.price = 323.1;
    order_4.first.quantity = 23;
    order_5.first.side = types::OrderInfo::Side::BUY;
    order_5.first.type = types::OrderInfo::Type::MARKET;
    order_5.first.price = 120.1;
    order_5.first.quantity = 23;
    order_6.first.side = types::OrderInfo::Side::SELL;
    order_6.first.type = types::OrderInfo::Type::MARKET;
    order_6.first.price = 3123.1;
    order_6.first.quantity = 23;
  }

  void TearDown() override { event_hub_.Shutdown(); }

  void SendOrderBookSnapshot(const market_stream::types::OrderBook& order_book) {
    event_hub_.dispatcher().lock()->DispatchEvent<MQ::Event::kNewSnapshotAvailable>(
        order_book);
    event_hub_.WaitForAllEventsProcessed();
  }

  void PlaceOrders() {
    order_1.second = real_market_emulator_->PlaceOrder(order_1.first);
    order_2.second = real_market_emulator_->PlaceOrder(order_2.first);
    order_3.second = real_market_emulator_->PlaceOrder(order_3.first);
    order_4.second = real_market_emulator_->PlaceOrder(order_4.first);
    order_5.second = real_market_emulator_->PlaceOrder(order_5.first);
    order_6.second = real_market_emulator_->PlaceOrder(order_6.first);
    ASSERT_EQ(real_market_emulator_->orders_count(), 6);
  }

  void PlaceOnlyMarketTypeOrders() {
    ASSERT_EQ(real_market_emulator_->orders_count(), 0);
    order_5.second = real_market_emulator_->PlaceOrder(order_5.first);
    order_6.second = real_market_emulator_->PlaceOrder(order_6.first);
    ASSERT_EQ(real_market_emulator_->orders_count(), 0);
  }

  void VerifyOrdersResults(const market_stream::types::OrderBook& ob_to_compare) {
    ASSERT_NE(order_1.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result1 = order_1.second.order_result.get();
    EXPECT_EQ(result1.status, types::OrderResult::Status::kOk);
    EXPECT_EQ(result1.price, order_1.first.price);
    EXPECT_EQ(result1.quantity, order_1.first.quantity);

    ASSERT_NE(order_2.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result2 = order_2.second.order_result.get();
    EXPECT_EQ(result2.status, types::OrderResult::Status::kOk);
    EXPECT_EQ(result2.price, order_2.first.price);
    EXPECT_EQ(result2.quantity, order_2.first.quantity);

    EXPECT_NE(order_3.second.order_result.wait_for(std::chrono::seconds(0)),
              std::future_status::ready);
    EXPECT_NE(order_4.second.order_result.wait_for(std::chrono::seconds(0)),
              std::future_status::ready);

    VerifyMarketTypeOrdersResults(ob_to_compare);
  }

  void VerifyMarketTypeOrdersResults(
      const market_stream::types::OrderBook& ob_to_compare) {
    ASSERT_NE(order_5.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result5 = order_5.second.order_result.get();
    EXPECT_EQ(result5.status, types::OrderResult::Status::kOk);
    EXPECT_EQ(result5.price, ob_to_compare.bids.front().price);
    EXPECT_EQ(result5.quantity, order_5.first.quantity);

    ASSERT_NE(order_6.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result6 = order_6.second.order_result.get();
    EXPECT_EQ(result6.status, types::OrderResult::Status::kOk);
    EXPECT_EQ(result6.price, ob_to_compare.asks.front().price);
    EXPECT_EQ(result6.quantity, order_6.first.quantity);
  }

  void VerifyOrdersResultsWithCanceled() {
    ASSERT_NE(order_1.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result1 = order_1.second.order_result.get();
    EXPECT_EQ(result1.status, types::OrderResult::Status::kOk);
    EXPECT_EQ(result1.price, order_1.first.price);
    EXPECT_EQ(result1.quantity, order_1.first.quantity);

    ASSERT_NE(order_2.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result2 = order_2.second.order_result.get();
    EXPECT_EQ(result2.status, types::OrderResult::Status::kCanceled);

    EXPECT_EQ(order_3.second.order_result.wait_for(std::chrono::seconds(0)),
              std::future_status::ready);
    EXPECT_EQ(order_3.second.order_result.get().status,
              types::OrderResult::Status::kCanceled);

    EXPECT_NE(order_4.second.order_result.wait_for(std::chrono::seconds(0)),
              std::future_status::ready);

    ASSERT_NE(order_5.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result5 = order_5.second.order_result.get();
    EXPECT_EQ(result5.status, types::OrderResult::Status::kCanceled);

    ASSERT_NE(order_6.second.order_result.wait_for(std::chrono::seconds(3)),
              std::future_status::timeout);
    auto result6 = order_6.second.order_result.get();
    EXPECT_EQ(result6.status, types::OrderResult::Status::kOk);
    EXPECT_EQ(result6.price, order_book1_.asks.front().price);
    EXPECT_EQ(result6.quantity, order_6.first.quantity);

    EXPECT_EQ(real_market_emulator_->orders_count(), 1);
  }

  std::shared_ptr<RealMarketEmulator> real_market_emulator_;
  std::pair<types::OrderInfo, types::OrderResponse<std::future>> order_1;
  std::pair<types::OrderInfo, types::OrderResponse<std::future>> order_2;
  std::pair<types::OrderInfo, types::OrderResponse<std::future>> order_3;
  std::pair<types::OrderInfo, types::OrderResponse<std::future>> order_4;
  std::pair<types::OrderInfo, types::OrderResponse<std::future>> order_5;
  std::pair<types::OrderInfo, types::OrderResponse<std::future>> order_6;

  std::shared_ptr<MockUnitState<ObservableUnits::UnitId::kRealMarketEmulator>>
      unit_state_;

  market_stream::types::OrderBook order_book1_;
  market_stream::types::OrderBook order_book2_;

 private:
  events::EventHub<events::message_queues::OrderBookStream> event_hub_;
};
}  // namespace

TEST_F(RealMarketEmulatorFixture,
       GivenPlacedOrders_WhenSendOrderBookSnapshot_ThenOrderResultsCorrect) {
  // Given
  testing::Sequence unit_state_seq;
  for (int i = 0; i < 6 + 1 + 2; i++) {  // 6 PlaceOrder, 1 Snapshot event, 2 orders_count
    EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
    EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);
  }
  PlaceOrders();

  // When
  SendOrderBookSnapshot(order_book1_);

  // Then
  VerifyOrdersResults(order_book1_);

  EXPECT_EQ(real_market_emulator_->orders_count(), 2);
}

TEST_F(RealMarketEmulatorFixture,
       GivenPlacedOrders_WhenSendTwoOrderBookSnapshot_ThenOrderResultsCorrect) {
  // Given
  testing::Sequence unit_state_seq;
  for (int i = 0; i < 2 + 2 + 3; i++) {  // 2 Snapshot, 2 PlaceOrder, 3 orders_count
    EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
    EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);
  }

  SendOrderBookSnapshot(order_book1_);
  PlaceOnlyMarketTypeOrders();

  // When
  SendOrderBookSnapshot(order_book2_);

  // Then
  VerifyMarketTypeOrdersResults(order_book1_);

  EXPECT_EQ(real_market_emulator_->orders_count(), 0);
}

TEST_F(RealMarketEmulatorFixture, GivenPlacedOrders_WhenCancelOrder_ThenOrdersCanceled) {
  // Given
  testing::Sequence unit_state_seq;
  for (int i = 0; i < 6 + 3 + 1 + 2;
       i++) {  // 6 PlaceOrder, 3 CancelOrder, 1 Snapshot event, 2 orders_count
    EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
    EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);
  }
  PlaceOrders();

  // When
  real_market_emulator_->CancelOrder(order_2.second.order_id.get());
  real_market_emulator_->CancelOrder(order_3.second.order_id.get());
  real_market_emulator_->CancelOrder(order_5.second.order_id.get());
  SendOrderBookSnapshot(order_book1_);

  // Then
  VerifyOrdersResultsWithCanceled();
}
}  // namespace analyzer
