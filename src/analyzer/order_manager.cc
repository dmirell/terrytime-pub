#include "analyzer/order_manager.h"

#include "analyzer/real_market_emulator.h"
#include "analyzer/scoped_unit_state.h"

namespace analyzer {

OrderManager::OrderManager(const std::shared_ptr<RealMarketEmulator> &market_emulator,
                           const std::shared_ptr<OrderManagerUnitState> &unit_state)
    : market_emulator_(market_emulator), unit_state_(unit_state) {}

types::OrderResponse<std::future> OrderManager::PlaceOrder(
    const types::OrderInfo &order_info) {
  ScopedUnitState<ObservableUnits::UnitId::kOrderManager> scoped_state(unit_state_);
  return market_emulator_->PlaceOrder(order_info);
}

void OrderManager::CancelOrder(uint64_t order_id) {
  ScopedUnitState<ObservableUnits::UnitId::kOrderManager> scoped_state(unit_state_);
  market_emulator_->CancelOrder(order_id);
}

}  // namespace analyzer