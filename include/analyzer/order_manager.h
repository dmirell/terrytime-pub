#ifndef INCLUDE_ANALYZER_ORDER_MANAGER_H_
#define INCLUDE_ANALYZER_ORDER_MANAGER_H_

#include <future>
#include <memory>

#include "analyzer/observable_units.h"
#include "types/types.h"

namespace analyzer {

class RealMarketEmulator;

class OrderManager {
 public:
  OrderManager() = delete;
  OrderManager(const OrderManager &) = delete;
  OrderManager(OrderManager &&) = delete;
  OrderManager &operator=(const OrderManager &) = delete;
  OrderManager &operator=(OrderManager &&) = delete;

  OrderManager(const std::shared_ptr<RealMarketEmulator> &market_emulator,
               const std::shared_ptr<OrderManagerUnitState> &unit_state);
  virtual ~OrderManager() = default;

  virtual types::OrderResponse<std::future> PlaceOrder(
      const types::OrderInfo &order_info);
  virtual void CancelOrder(uint64_t order_id);

 private:
  std::shared_ptr<RealMarketEmulator> market_emulator_;
  std::shared_ptr<OrderManagerUnitState> unit_state_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_ORDER_MANAGER_H_