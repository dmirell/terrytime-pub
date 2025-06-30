#include <gmock/gmock.h>

#include <chrono>
#include <thread>

#include "analyzer/benchmark_orchestrator.h"
#include "events/event_hub.h"
#include "market_stream/saved_market_stream_forwarder.h"
#include "utils/tests/helpers/scoped_logger.h"
#include "utils/time/global_clock.h"
#include "utils/time/mock_time_provider.h"

namespace analyzer {
namespace {
class MockSavedMarketStreamForwarder : public market_stream::SavedMarketStreamForwarder {
  using MQ = events::message_queues::MarketStream;
  using EventHubDispatcher = events::EventHub<MQ>::Dispatcher;

 public:
  MockSavedMarketStreamForwarder(const std::weak_ptr<EventHubDispatcher>& dispatcher)
      : SavedMarketStreamForwarder("", dispatcher) {}
  MOCK_METHOD(void, Initialize, (), (override));
  MOCK_METHOD(bool, ReadNext, (utils::Timestamp*), (override));
  MOCK_METHOD(void, ForwardNext, (), (override));
};

class MockMockTimeProvider : public utils::MockTimeProvider {
 public:
  MockMockTimeProvider(utils::Timestamp start_time)
      : utils::MockTimeProvider(start_time) {}
  MOCK_METHOD(void, WaitUntil, (utils::Timestamp), (override));
  MOCK_METHOD(void, JumpToFirstAwaitingTimestamp, (), (override));
};

class BenchmarkOrchestratorFixture : public ::testing::Test {
  using MQ = events::message_queues::MarketStream;

 public:
  BenchmarkOrchestratorFixture() {
    saved_market_stream_forwarder_mock_ =
        std::make_shared<MockSavedMarketStreamForwarder>(event_hub_.dispatcher());
    benchmark_orchestrator_ =
        std::make_shared<BenchmarkOrchestrator>(saved_market_stream_forwarder_mock_);

    analyzer_state_ =
        std::make_unique<ObservableUnits::Unit<ObservableUnits::UnitId::kAnalyzer>>();
    order_plan_manager_state_ = std::make_unique<
        ObservableUnits::Unit<ObservableUnits::UnitId::kOrderPlanManager>>();
    real_market_emulator_state_ = std::make_unique<
        ObservableUnits::Unit<ObservableUnits::UnitId::kRealMarketEmulator>>();
    forwarder_state_ = std::make_unique<
        ObservableUnits::Unit<ObservableUnits::UnitId::kStreamForwarder>>();
    snapshot_provider_state_ = std::make_unique<
        ObservableUnits::Unit<ObservableUnits::UnitId::kOrderBookSnapshotProvider>>();
    order_manager_state_ =
        std::make_unique<ObservableUnits::Unit<ObservableUnits::UnitId::kOrderManager>>();
    transport_state_ =
        std::make_unique<ObservableUnits::Unit<ObservableUnits::UnitId::kTransport>>();
  }

  void GoAsync() {
    benchmark_orchestrator_thread_ =
        std::thread([this]() { benchmark_orchestrator_->Go(); });
  }

  void SetUp() override {
    analyzer_state_->SetReady();
    order_plan_manager_state_->SetReady();
    real_market_emulator_state_->SetReady();
    forwarder_state_->SetReady();
    snapshot_provider_state_->SetReady();
    order_manager_state_->SetReady();
    transport_state_->SetReady();
  }

  std::shared_ptr<BenchmarkOrchestrator> benchmark_orchestrator_;
  std::shared_ptr<MockSavedMarketStreamForwarder> saved_market_stream_forwarder_mock_;

  std::unique_ptr<ObservableUnits::Unit<ObservableUnits::UnitId::kAnalyzer>>
      analyzer_state_;
  std::unique_ptr<ObservableUnits::Unit<ObservableUnits::UnitId::kOrderPlanManager>>
      order_plan_manager_state_;
  std::unique_ptr<ObservableUnits::Unit<ObservableUnits::UnitId::kRealMarketEmulator>>
      real_market_emulator_state_;
  std::unique_ptr<ObservableUnits::Unit<ObservableUnits::UnitId::kStreamForwarder>>
      forwarder_state_;
  std::unique_ptr<
      ObservableUnits::Unit<ObservableUnits::UnitId::kOrderBookSnapshotProvider>>
      snapshot_provider_state_;
  std::unique_ptr<ObservableUnits::Unit<ObservableUnits::UnitId::kOrderManager>>
      order_manager_state_;
  std::unique_ptr<ObservableUnits::Unit<ObservableUnits::UnitId::kTransport>>
      transport_state_;

 private:
  void TearDown() override {
    if (benchmark_orchestrator_thread_.joinable()) {
      benchmark_orchestrator_thread_.join();
    }
  }

  events::EventHub<MQ> event_hub_;
  std::thread benchmark_orchestrator_thread_;
};
}  // namespace

TEST_F(BenchmarkOrchestratorFixture,
       GivenRunBenchmarkOrchestrator_WhenNotReadyAllUnitsAndForwardNext_ThenWarning) {
  utils::ScopedLogger logger(spdlog::level::warn);

  // Given
  testing::Sequence seq;
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this](utils::Timestamp* data_ts) {
        *data_ts = 0;
        order_plan_manager_state_->SetBusy();
        real_market_emulator_state_->SetBusy();

        return true;
      });
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ForwardNext()).InSequence(seq);
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this](utils::Timestamp*) { return false; });
  // When
  GoAsync();

  std::this_thread::sleep_for(std::chrono::milliseconds(1050));

  // Now should be set at first data ts
  EXPECT_GE(utils::GlobalClock::Instance().Now(), 1000);
  EXPECT_LE(utils::GlobalClock::Instance().Now(), 1100);

  order_plan_manager_state_->SetReady();

  std::this_thread::sleep_for(std::chrono::seconds(1));
  benchmark_orchestrator_->Stop();

  // Then
  EXPECT_NE(logger.str().find("forwarding event while some of units is not ready"),
            std::string::npos);
}

TEST_F(BenchmarkOrchestratorFixture,
       GivenRunBenchmarkOrchestrator_WhenReadyAllUnits_ThenJumpToNextTs) {
  utils::ScopedLogger logger(spdlog::level::warn);

  // Given
  utils::Timestamp next_data_ts = 10000;
  testing::Sequence seq;
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this](utils::Timestamp* data_ts) {
        *data_ts = 0;

        return true;
      });
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ForwardNext()).InSequence(seq);
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this, next_data_ts](utils::Timestamp* data_ts) {
        *data_ts = next_data_ts;
        order_plan_manager_state_->SetBusy();
        real_market_emulator_state_->SetBusy();

        return true;
      });
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ForwardNext()).InSequence(seq);
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this](utils::Timestamp*) { return false; });
  // When
  GoAsync();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Now should be set at first data ts
  EXPECT_GE(utils::GlobalClock::Instance().Now(), 900);
  EXPECT_LE(utils::GlobalClock::Instance().Now(), 1100);

  EXPECT_EQ(ObservableUnits::Instance().GetBusyUnits().size(), 2);

  order_plan_manager_state_->SetReady();
  real_market_emulator_state_->SetReady();

  EXPECT_TRUE(ObservableUnits::Instance().GetBusyUnits().empty());

  std::this_thread::sleep_for(std::chrono::seconds(1));
  benchmark_orchestrator_->Stop();

  EXPECT_EQ(logger.str().find("forwarding event while some of units is not ready"),
            std::string::npos);
}

TEST_F(BenchmarkOrchestratorFixture,
       GivenRunBenchmarkOrchestrator_WhenNotReadyAllUnits_ThenWaitForNextTs) {
  utils::ScopedLogger logger(spdlog::level::warn);

  // Given
  utils::Timestamp next_data_ts = 5000;
  utils::Timestamp real_next_data_read_ts = 0;
  testing::Sequence seq;
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this](utils::Timestamp* data_ts) {
        *data_ts = 0;

        return true;
      });
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ForwardNext()).InSequence(seq);
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this, next_data_ts](utils::Timestamp* data_ts) {
        *data_ts = next_data_ts;
        order_plan_manager_state_->SetBusy();
        real_market_emulator_state_->SetBusy();

        return true;
      });
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ForwardNext()).InSequence(seq);
  EXPECT_CALL(*saved_market_stream_forwarder_mock_, ReadNext(testing::_))
      .InSequence(seq)
      .WillOnce([this, &real_next_data_read_ts](utils::Timestamp*) {
        real_next_data_read_ts = utils::GlobalClock::Instance().Now();
        return false;
      });
  // When
  GoAsync();

  std::this_thread::sleep_for(std::chrono::seconds(1));

  // Now should be set at first data ts
  EXPECT_GE(utils::GlobalClock::Instance().Now(), 900);
  EXPECT_LE(utils::GlobalClock::Instance().Now(), 1100);

  order_plan_manager_state_->SetReady();

  std::this_thread::sleep_for(std::chrono::seconds(7));

  // Then

  EXPECT_GE(real_next_data_read_ts, next_data_ts - 100);
  EXPECT_LE(real_next_data_read_ts, next_data_ts + 100);
  EXPECT_NE(logger.str().find("forwarding event while some of units is not ready"),
            std::string::npos);
  benchmark_orchestrator_->Stop();
}

}  // namespace analyzer