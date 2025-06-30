#include <gmock/gmock.h>

#include "analyzer/benchmark_orchestrator.h"
#include "analyzer/order_manager.h"
#include "analyzer/real_market_emulator.h"
#include "events/event_hub.h"
#include "utils/tests/helpers/unit_state_mock.h"

namespace analyzer {

namespace {
class MockRealMarketEmulator : public analyzer::RealMarketEmulator {
 public:
  MockRealMarketEmulator(
      const std::weak_ptr<
          events::EventHub<events::message_queues::OrderBookStream>::Handler>& handler)
      : analyzer::RealMarketEmulator(handler, nullptr) {}
  MOCK_METHOD(analyzer::types::OrderResponse<std::future>, PlaceOrder,
              (const analyzer::types::OrderInfo& order_info), (override));
  MOCK_METHOD(void, CancelOrder, (uint64_t order_id), (override));
};

class OrderManagerFixture : public ::testing::Test {
 public:
  OrderManagerFixture() {
    unit_state_ =
        std::make_shared<MockUnitState<ObservableUnits::UnitId::kOrderManager>>();
    real_market_emulator_mock_ =
        std::make_shared<MockRealMarketEmulator>(event_hub_.CreateHandler());
    order_manager_ =
        std::make_shared<analyzer::OrderManager>(real_market_emulator_mock_, unit_state_);
  }

  std::shared_ptr<analyzer::OrderManager> order_manager() { return order_manager_; }
  std::shared_ptr<MockRealMarketEmulator> real_market_emulator_mock() {
    return real_market_emulator_mock_;
  }

  std::shared_ptr<MockUnitState<ObservableUnits::UnitId::kOrderManager>> unit_state_;

 private:
  events::EventHub<events::message_queues::OrderBookStream> event_hub_;
  std::shared_ptr<analyzer::OrderManager> order_manager_;
  std::shared_ptr<MockRealMarketEmulator> real_market_emulator_mock_;
};
}  // namespace

TEST_F(OrderManagerFixture, GivenOrderInfo_WhenPlaceOrder_ThenRMEPlaceOrderCalled) {
  // Given

  analyzer::types::OrderInfo order_info;
  order_info.price = 2;
  order_info.quantity = 2.5;

  // When
  EXPECT_CALL(*real_market_emulator_mock(), PlaceOrder(order_info));

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);

  order_manager()->PlaceOrder(order_info);
}

TEST_F(OrderManagerFixture, GivenOrderId_WhenCancelOrder_ThenRMECancelOrderCalled) {
  // Given
  uint64_t order_id = 123456;

  // When
  EXPECT_CALL(*real_market_emulator_mock(), CancelOrder(order_id));

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);

  order_manager()->CancelOrder(order_id);
}
}  // namespace analyzer