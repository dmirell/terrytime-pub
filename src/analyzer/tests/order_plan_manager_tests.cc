#include <gmock/gmock.h>

#include <chrono>
#include <future>
#include <thread>

#include "analyzer/benchmark_data_collector.h"
#include "analyzer/benchmark_orchestrator.h"
#include "analyzer/order_manager.h"
#include "analyzer/order_plan_manager.h"
#include "analyzer/types/types.h"
#include "events/event_hub.h"
#include "utils/tests/helpers/scoped_logger.h"
#include "utils/tests/helpers/unit_state_mock.h"
#include "utils/time/global_clock.h"
#include "utils/time/mock_time_provider.h"

namespace analyzer {

namespace {

class MockOrderManager : public OrderManager {
 public:
  MockOrderManager() : OrderManager(nullptr, nullptr) {}
  MOCK_METHOD(types::OrderResponse<std::future>, PlaceOrder,
              (const types::OrderInfo& order_info), (override));
  MOCK_METHOD(void, CancelOrder, (uint64_t order_id), (override));
};

class MockBenchmarkDataCollector : public BenchmarkDataCollector {
 public:
  MOCK_METHOD(void, AddOrderPlanReport, (const types::OrderPlanReport& report),
              (override));
};

class OrderPlanManagerFixture : public ::testing::Test {
  using MQ = events::message_queues::AnalyzerStream;
  using EventHubHandler = events::EventHub<MQ>::Handler;

 public:
  OrderPlanManagerFixture() {
    unit_state_ =
        std::make_shared<MockUnitState<ObservableUnits::UnitId::kOrderPlanManager>>();
    order_manager_ = std::make_shared<MockOrderManager>();
    benchmark_data_collector_ = std::make_shared<MockBenchmarkDataCollector>();
    order_plan_manager_ =
        std::make_shared<OrderPlanManager>(event_hub_.CreateHandler(), order_manager_,
                                           unit_state_, benchmark_data_collector_);

    // Setting up mock time provider for tests
    mock_time_provider_ = std::make_shared<utils::MockTimeProvider>(0);
    utils::GlobalClock::Instance().SetTimeProvider(mock_time_provider_);
  }

  void SendOrderPlanInfo(const types::OrderPlanInfo& order_plan) {
    event_hub_.dispatcher().lock()->DispatchEvent<MQ::Event::kNewOrderPlan>(order_plan);
  }

  std::shared_ptr<MockOrderManager> order_manager_;
  std::shared_ptr<utils::MockTimeProvider> mock_time_provider_;
  std::shared_ptr<MockBenchmarkDataCollector> benchmark_data_collector_;
  events::EventHub<events::message_queues::AnalyzerStream> event_hub_;

  std::shared_ptr<MockUnitState<ObservableUnits::UnitId::kOrderPlanManager>> unit_state_;

 private:
  std::shared_ptr<OrderPlanManager> order_plan_manager_;
};
}  // namespace

TEST_F(OrderPlanManagerFixture,
       GivenOrderPlanInfo_WhenOrderPlanExperied_ThenOrderPlanSkipped) {
  // Given
  utils::ScopedLogger logger(spdlog::level::warn);

  types::OrderPlanInfo order_plan;
  order_plan.expiration_ts = utils::GlobalClock::Instance().Now();

  // When
  std::this_thread::sleep_for(utils::ChronoTimestampPrecision(1200));

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);

  types::OrderPlanReport expected_report;
  expected_report.buy_price = 0;
  expected_report.sold_price = 0;
  expected_report.status = types::OrderPlanReport::Status::kPlanExpired;
  EXPECT_CALL(*benchmark_data_collector_, AddOrderPlanReport(expected_report));

  SendOrderPlanInfo(order_plan);
  event_hub_.Shutdown();

  // Make a pause to be sure that order plan become processed
  std::this_thread::sleep_for(utils::ChronoTimestampPrecision(1200));

  // Then
  std::string test = logger.str();
  EXPECT_NE(test.find("Order plan has expired"), std::string::npos);
}

TEST_F(OrderPlanManagerFixture, GivenOrderPlanInfo_WhenBuyExperied_ThenBuyOrderCanceled) {
  // Given
  utils::ScopedLogger logger(spdlog::level::warn);

  types::OrderPlanInfo order_plan;
  order_plan.expiration_ts = utils::GlobalClock::Instance().Now() + 1000000;
  order_plan.expiration_buy_ts = utils::GlobalClock::Instance().Now() + 2000;
  order_plan.max_buy_price = 12.34;

  types::OrderInfo expected_buy_order;
  expected_buy_order.price = order_plan.max_buy_price;
  expected_buy_order.side = types::OrderInfo::Side::BUY;
  expected_buy_order.type = types::OrderInfo::Type::LIMIT;

  types::OrderResponse<std::promise> expected_buy_responce_gen;

  // When
  testing::Sequence seq;
  EXPECT_CALL(*order_manager_, PlaceOrder(expected_buy_order))
      .InSequence(seq)
      .WillOnce([&]() {
        types::OrderResponse<std::future> expected_responce;
        expected_responce.order_id = expected_buy_responce_gen.order_id.get_future();
        expected_responce.order_result =
            expected_buy_responce_gen.order_result.get_future();
        expected_buy_responce_gen.order_id.set_value(123456);
        return expected_responce;
      });
  EXPECT_CALL(*order_manager_, CancelOrder(123456)).InSequence(seq).WillOnce([&]() {
    types::OrderResult result;
    result.status = types::OrderResult::Status::kCanceled;
    expected_buy_responce_gen.order_result.set_value(result);
  });

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);

  types::OrderPlanReport expected_report;
  expected_report.buy_price = 0;
  expected_report.sold_price = 0;
  expected_report.status = types::OrderPlanReport::Status::kFailedToBuy;
  EXPECT_CALL(*benchmark_data_collector_, AddOrderPlanReport(expected_report));

  SendOrderPlanInfo(order_plan);

  // Call twice - should dropped due to previous in process
  SendOrderPlanInfo(order_plan);

  event_hub_.Shutdown();

  // Need this sleep because order plan manager runs order processing asyncronically
  std::this_thread::sleep_for(std::chrono::seconds(3));

  // Then
  EXPECT_NE(logger.str().find("Failed execute BUY order"), std::string::npos);
  EXPECT_NE(logger.str().find("Drop order plan due to another in process"),
            std::string::npos);
}

TEST_F(OrderPlanManagerFixture,
       GivenOrderPlanInfo_WhenSellExperied_ThenSellOrderCanceledAndSoldAsset) {
  // Given
  utils::ScopedLogger logger(spdlog::level::warn);

  types::OrderPlanInfo order_plan;
  order_plan.expiration_ts = utils::GlobalClock::Instance().Now() + 1000000;
  order_plan.expiration_buy_ts = utils::GlobalClock::Instance().Now() + 1000000;
  order_plan.expiration_sell_ts = utils::GlobalClock::Instance().Now() + 6000;
  order_plan.max_buy_price = 12.34;
  order_plan.min_sell_price = 13.34;

  types::OrderInfo expected_buy_order;
  expected_buy_order.price = order_plan.max_buy_price;
  expected_buy_order.side = types::OrderInfo::Side::BUY;
  expected_buy_order.type = types::OrderInfo::Type::LIMIT;

  types::OrderInfo expected_sell_order;
  expected_sell_order.price = order_plan.min_sell_price;
  expected_sell_order.side = types::OrderInfo::Side::SELL;
  expected_sell_order.type = types::OrderInfo::Type::LIMIT;

  types::OrderInfo expected_market_sell_order;
  expected_market_sell_order.side = types::OrderInfo::Side::SELL;
  expected_market_sell_order.type = types::OrderInfo::Type::MARKET;

  types::OrderResponse<std::promise> expected_buy_responce_gen;
  types::OrderResponse<std::promise> expected_sell_responce_gen;
  types::OrderResponse<std::promise> expected_market_sell_responce_gen;

  // When
  testing::Sequence seq;
  EXPECT_CALL(*order_manager_, PlaceOrder(expected_buy_order))
      .InSequence(seq)
      .WillOnce([&]() {
        types::OrderResponse<std::future> expected_responce;
        expected_responce.order_id = expected_buy_responce_gen.order_id.get_future();
        expected_responce.order_result =
            expected_buy_responce_gen.order_result.get_future();

        const auto order_id = 123456;
        expected_buy_responce_gen.order_id.set_value(order_id);

        types::OrderResult result;
        result.status = types::OrderResult::Status::kOk;
        result.order_id = order_id;
        result.price = 11.34;
        expected_buy_responce_gen.order_result.set_value(result);
        return expected_responce;
      });
  EXPECT_CALL(*order_manager_, PlaceOrder(expected_sell_order))
      .InSequence(seq)
      .WillOnce([&]() {
        types::OrderResponse<std::future> expected_responce;
        expected_responce.order_id = expected_sell_responce_gen.order_id.get_future();
        expected_responce.order_result =
            expected_sell_responce_gen.order_result.get_future();
        const auto order_id = 123457;
        expected_sell_responce_gen.order_id.set_value(order_id);
        return expected_responce;
      });
  EXPECT_CALL(*order_manager_, CancelOrder(123457)).InSequence(seq).WillOnce([&]() {
    types::OrderResult result;
    result.status = types::OrderResult::Status::kCanceled;
    expected_sell_responce_gen.order_result.set_value(result);
  });
  EXPECT_CALL(*order_manager_, PlaceOrder(expected_market_sell_order))
      .InSequence(seq)
      .WillOnce([&]() {
        types::OrderResponse<std::future> expected_responce;
        expected_responce.order_id =
            expected_market_sell_responce_gen.order_id.get_future();
        expected_responce.order_result =
            expected_market_sell_responce_gen.order_result.get_future();

        const auto order_id = 123458;
        expected_market_sell_responce_gen.order_id.set_value(order_id);

        types::OrderResult result;
        result.status = types::OrderResult::Status::kOk;
        result.order_id = order_id;
        result.price = 13.00;
        expected_market_sell_responce_gen.order_result.set_value(result);
        return expected_responce;
      });

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);

  types::OrderPlanReport expected_report;
  expected_report.buy_price = 11.34;
  expected_report.sold_price = 13.00;
  expected_report.status = types::OrderPlanReport::Status::kFailedToProfitSell;
  EXPECT_CALL(*benchmark_data_collector_, AddOrderPlanReport(expected_report));

  SendOrderPlanInfo(order_plan);
  event_hub_.Shutdown();

  // Need this sleep because order plan manager runs order processing asyncronically
  std::this_thread::sleep_for(std::chrono::seconds(7));

  // Then
  EXPECT_NE(logger.str().find("Failed execute SELL order"), std::string::npos);
}

TEST_F(OrderPlanManagerFixture,
       GivenOrderPlanInfo_WhenAllFinished_ThenOrderPlanFinishedSuccessfully) {
  // Given
  utils::ScopedLogger logger(spdlog::level::info);

  types::OrderPlanInfo order_plan;
  order_plan.expiration_ts = utils::GlobalClock::Instance().Now() + 1000000;
  order_plan.expiration_buy_ts = utils::GlobalClock::Instance().Now() + 1000000;
  order_plan.expiration_sell_ts = utils::GlobalClock::Instance().Now() + 1000000;
  order_plan.max_buy_price = 12.34;
  order_plan.min_sell_price = 13.34;

  types::OrderInfo expected_buy_order;
  expected_buy_order.price = order_plan.max_buy_price;
  expected_buy_order.side = types::OrderInfo::Side::BUY;
  expected_buy_order.type = types::OrderInfo::Type::LIMIT;

  types::OrderInfo expected_sell_order;
  expected_sell_order.price = order_plan.min_sell_price;
  expected_sell_order.side = types::OrderInfo::Side::SELL;
  expected_sell_order.type = types::OrderInfo::Type::LIMIT;

  types::OrderResponse<std::promise> expected_buy_responce_gen;
  types::OrderResponse<std::promise> expected_sell_responce_gen;

  // When
  testing::Sequence seq;
  EXPECT_CALL(*order_manager_, PlaceOrder(expected_buy_order))
      .InSequence(seq)
      .WillOnce([&]() {
        types::OrderResponse<std::future> expected_responce;
        expected_responce.order_id = expected_buy_responce_gen.order_id.get_future();
        expected_responce.order_result =
            expected_buy_responce_gen.order_result.get_future();

        const auto order_id = 123456;
        expected_buy_responce_gen.order_id.set_value(order_id);

        types::OrderResult result;
        result.status = types::OrderResult::Status::kOk;
        result.order_id = order_id;
        result.price = 11.34;
        expected_buy_responce_gen.order_result.set_value(result);
        return expected_responce;
      });
  EXPECT_CALL(*order_manager_, PlaceOrder(expected_sell_order))
      .InSequence(seq)
      .WillOnce([&]() {
        types::OrderResponse<std::future> expected_responce;
        expected_responce.order_id = expected_sell_responce_gen.order_id.get_future();
        expected_responce.order_result =
            expected_sell_responce_gen.order_result.get_future();

        const auto order_id = 123457;
        expected_sell_responce_gen.order_id.set_value(order_id);

        types::OrderResult result;
        result.status = types::OrderResult::Status::kOk;
        result.order_id = order_id;
        result.price = 14.56;
        expected_sell_responce_gen.order_result.set_value(result);
        return expected_responce;
      });

  testing::Sequence unit_state_seq;
  EXPECT_CALL(*unit_state_, SetBusy()).InSequence(unit_state_seq);
  EXPECT_CALL(*unit_state_, SetReady()).InSequence(unit_state_seq);

  types::OrderPlanReport expected_report;
  expected_report.buy_price = 11.34;
  expected_report.sold_price = 14.56;
  expected_report.status = types::OrderPlanReport::Status::kOk;
  EXPECT_CALL(*benchmark_data_collector_, AddOrderPlanReport(expected_report));

  SendOrderPlanInfo(order_plan);
  event_hub_.Shutdown();

  // Need this sleep because order plan manager runs order processing asyncronically
  std::this_thread::sleep_for(std::chrono::seconds(7));

  // Then
  EXPECT_NE(logger.str().find("OrderPlan processing finishe"), std::string::npos);
}
}  // namespace analyzer