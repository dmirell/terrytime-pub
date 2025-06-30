#include "analyzer/real_market_emulator.h"

#include <spdlog/spdlog.h>

#include <algorithm>

#include "analyzer/scoped_unit_state.h"
#include "utils/time/global_clock.h"

namespace analyzer {

namespace {
uint64_t unique_id_iterator = 0;
}  // namespace

RealMarketEmulator::RealMarketEmulator(
    const std::weak_ptr<EventHubHandler> &event_handler,
    const std::shared_ptr<RealMarketEmulatorUnitState> &unit_state)
    : last_max_buy_(-1), last_min_sell_(-1), unit_state_(unit_state) {
  SUBSCRIBE_TO_EVENT(event_handler, &RealMarketEmulator::OnOrderBookStreamEvent);
}

types::OrderResponse<std::future> RealMarketEmulator::PlaceOrder(
    const types::OrderInfo &order_info) {
  ScopedUnitState<ObservableUnits::UnitId::kRealMarketEmulator> scoped_state(unit_state_);
  std::lock_guard<std::mutex> lock(mutex_);
  return PlaceOrder_locked(order_info);
}

void RealMarketEmulator::CancelOrder(uint64_t order_id) {
  ScopedUnitState<ObservableUnits::UnitId::kRealMarketEmulator> scoped_state(unit_state_);
  std::lock_guard<std::mutex> lock(mutex_);
  CancelOrder_locked(order_id);
}

std::size_t RealMarketEmulator::orders_count() const {
  ScopedUnitState<ObservableUnits::UnitId::kRealMarketEmulator> scoped_state(unit_state_);
  std::lock_guard<std::mutex> lock(mutex_);
  return orders_count_locked();
}

void RealMarketEmulator::OnOrderBookStreamEvent(MQ::Event event, const void *data) {
  if (MQ::Event::kNewSnapshotAvailable != event) {
    spdlog::error("unknown event type");
    return;
  }
  ScopedUnitState<ObservableUnits::UnitId::kRealMarketEmulator> scoped_state(unit_state_);
  std::lock_guard<std::mutex> lock(mutex_);
  OnOrderBookStreamEvent_locked(event, data);
}

types::OrderResponse<std::future> RealMarketEmulator::PlaceOrder_locked(
    const types::OrderInfo &order_info) {
  types::OrderResponse<std::future> responce;
  Order order;
  order.id = ++unique_id_iterator;
  order.info = order_info;
  responce.order_id = order.responce.order_id.get_future();
  responce.order_result = order.responce.order_result.get_future();

  order.responce.order_id.set_value(order.id);

  orders_.emplace_back(std::move(order));

  // For order type MARKET
  auto finished_market_orders = ProcessMarketTypeOrders();
  RemoveFinishedOrders(finished_market_orders);

  return responce;
}

void RealMarketEmulator::CancelOrder_locked(uint64_t order_id) {
  if (!CancelOrderById(order_id)) {
    spdlog::error("Failed to cancel order: {}", order_id);
  }
}

std::size_t RealMarketEmulator::orders_count_locked() const { return orders_.size(); }

void RealMarketEmulator::OnOrderBookStreamEvent_locked(MQ::Event event,
                                                       const void *data) {
  auto order_book = static_cast<const market_stream::types::OrderBook *>(data);
  spdlog::debug("OrderBookStreamEvent: recieved_ts={}, current_ts={}",
                order_book->received_timestamp, utils::GlobalClock::Instance().Now());
  last_max_buy_ = order_book->bids.front().price;
  last_min_sell_ = order_book->asks.front().price;
  auto finished_orders = CheckAndUpdateOrders(last_max_buy_, last_min_sell_);
  RemoveFinishedOrders(finished_orders);
}

std::vector<uint64_t> RealMarketEmulator::ProcessMarketTypeOrders() {
  std::vector<uint64_t> finished_orders;
  if (last_max_buy_ < 0 || last_min_sell_ < 0) {
    spdlog::warn("process market orders while zero market snapshot recieved");
    return finished_orders;
  }
  for (auto &it : orders_) {
    if (types::OrderInfo::Type::MARKET == it.info.type) {
      spdlog::info("Found finished market order_id: {}", it.id);
      types::OrderResult result;
      result.status = types::OrderResult::Status::kOk;
      result.order_id = it.id;
      result.price =
          types::OrderInfo::Side::BUY == it.info.side ? last_max_buy_ : last_min_sell_;
      result.quantity = it.info.quantity;
      it.responce.order_result.set_value(result);
      finished_orders.push_back(it.id);
      continue;
    }
  }
  return finished_orders;
}

std::vector<uint64_t> RealMarketEmulator::CheckAndUpdateOrders(
    market_stream::types::DoubleType max_buy, market_stream::types::DoubleType min_sell) {
  std::vector<uint64_t> finished_orders = ProcessMarketTypeOrders();
  for (auto &it : orders_) {
    const bool is_finished_as_buy_order =
        types::OrderInfo::Side::BUY == it.info.side && it.info.price >= max_buy;
    const bool is_finished_as_sell_order =
        types::OrderInfo::Side::SELL == it.info.side && it.info.price <= min_sell;
    if (is_finished_as_buy_order || is_finished_as_sell_order) {
      spdlog::info("Found finished limit order {}", it.id);
      types::OrderResult result;
      result.status = types::OrderResult::Status::kOk;
      result.order_id = it.id;
      result.price = it.info.price;
      result.quantity = it.info.quantity;
      it.responce.order_result.set_value(result);
      finished_orders.push_back(it.id);
    }
  }
  return finished_orders;
}

void RealMarketEmulator::RemoveFinishedOrders(const std::vector<uint64_t> &ids) {
  for (const auto order_id : ids) {
    auto it_found = FindOrderById(order_id);
    if (it_found != orders_.end()) {
      orders_.erase(it_found);
    } else {
      spdlog::error("Was not found finished order to remove from list");
    }
  }
}

std::vector<RealMarketEmulator::Order>::iterator RealMarketEmulator::FindOrderById(
    uint64_t order_id) {
  return std::find_if(orders_.begin(), orders_.end(),
                      [order_id](const Order &order) { return order.id == order_id; });
}

bool RealMarketEmulator::CancelOrderById(uint64_t order_id) {
  auto it = FindOrderById(order_id);
  if (it != orders_.end()) {
    types::OrderResult result;
    result.status = types::OrderResult::Status::kCanceled;
    result.order_id = it->id;
    result.price = it->info.price;
    result.quantity = it->info.quantity;
    it->responce.order_result.set_value(result);
    orders_.erase(it);
    return true;
  }
  return false;
}

}  // namespace analyzer