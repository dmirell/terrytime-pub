#include <gmock/gmock.h>

#include "analyzer/observable_units.h"
#include "events/event_hub.h"

namespace analyzer {

namespace {
class ObservableUnitsFixture : public ::testing::Test {
  using MQ = events::message_queues::MarketStream;

 public:
  ObservableUnitsFixture() {
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

  void SetUp() override {
    analyzer_state_->SetReady();
    order_plan_manager_state_->SetReady();
    real_market_emulator_state_->SetReady();
    forwarder_state_->SetReady();
    snapshot_provider_state_->SetReady();
    order_manager_state_->SetReady();
    transport_state_->SetReady();
  }

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
};
}  // namespace

TEST_F(ObservableUnitsFixture,
       GivenBusyFewUnits_WhenUnitsReady_ThenAllUnitsReadCbCalled) {
  // Given
  analyzer_state_->SetBusy();
  real_market_emulator_state_->SetBusy();

  int all_units_ready_event = 0;
  auto busy_units = ObservableUnits::Instance().GetBusyUnits();
  ObservableUnits::Instance().SubscribeToAllUnitsReady(
      [&]() { all_units_ready_event++; });

  ASSERT_EQ(busy_units.size(), 2);
  ASSERT_NE(
      std::find(busy_units.begin(), busy_units.end(), ObservableUnits::UnitId::kAnalyzer),
      busy_units.end());
  ASSERT_NE(std::find(busy_units.begin(), busy_units.end(),
                      ObservableUnits::UnitId::kRealMarketEmulator),
            busy_units.end());

  // When
  analyzer_state_->SetReady();
  real_market_emulator_state_->SetReady();
  real_market_emulator_state_
      ->SetReady();  // second all units read event here is expected

  ObservableUnits::Instance().Unsubscribe();

  real_market_emulator_state_->SetReady();

  // Then
  EXPECT_EQ(all_units_ready_event, 2);
}

}  // namespace analyzer