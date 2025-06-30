#include "analyzer/order_plan_manager.h"

#include "analyzer/benchmark_data_collector.h"
#include "analyzer/order_manager.h"
#include "analyzer/scoped_unit_state.h"
#include "utils/time/elapsed_time_meter.h"
#include "utils/time/global_clock.h"

namespace analyzer {

namespace {
const auto kMaxCancelResponceTime = std::chrono::seconds(60);
}

OrderPlanManager::OrderPlanManager(
    const std::weak_ptr<EventHubHandler> &event_handler,
    const std::shared_ptr<OrderManager> &order_manager,
    const std::shared_ptr<OrderPlanManagerUnitState> &unit_state,
    const std::shared_ptr<BenchmarkDataCollector> &benchmark_data_collector)
    : order_manager_(order_manager),
      unit_state_(unit_state),
      benchmark_data_collector_(benchmark_data_collector) {
  SUBSCRIBE_TO_EVENT(event_handler, &OrderPlanManager::OnOrderPlanStreamEvent);
  order_plans_processing_thread_ =
      std::thread(&OrderPlanManager::OrderPlansProcessThread, this);
}

OrderPlanManager::~OrderPlanManager() {
  is_stopped_ = true;
  cv_.notify_one();
  order_plans_processing_thread_.detach();
}

void OrderPlanManager::OnOrderPlanStreamEvent(MQ::Event event, const void *data) {
  if (MQ::Event::kNewOrderPlan != event) {
    spdlog::error("Unknown event type");
    return;
  }

  auto order_plan = static_cast<const types::OrderPlanInfo *>(data);
  spdlog::info("New order plan recieved");
  if (order_plan_info_ == nullptr) {
    order_plan_info_ = std::make_shared<types::OrderPlanInfo>(*order_plan);
    cv_.notify_one();
  } else {
    spdlog::warn("Drop order plan due to another in process");
  }
}

void OrderPlanManager::ProcessOrderPlan(types::OrderPlanInfo order_plan) {
  ScopedUnitState<ObservableUnits::UnitId::kOrderPlanManager> scoped_state(unit_state_);

  const auto order_plan_start_time = utils::GlobalClock::Instance().Now();
  utils::ElapsedTimeMeter elapsed_time_meter;

  // If order plan expired
  if (order_plan.expiration_ts <= order_plan_start_time) {
    spdlog::warn("Order plan has expired");
    SendReport(0, 0, order_plan_start_time, elapsed_time_meter.elapsed_time().count(),
               types::OrderPlanReport::Status::kPlanExpired);
    return;
  }

  spdlog::info("Starting processing...");

  types::OrderInfo buy_order;
  types::OrderResult buy_order_result;
  buy_order.price = order_plan.max_buy_price;
  buy_order.side = types::OrderInfo::Side::BUY;
  buy_order.type = types::OrderInfo::Type::LIMIT;
  if (!ExecOrder(buy_order, order_plan.expiration_buy_ts, &buy_order_result)) {
    spdlog::warn("Failed execute BUY order");
    SendReport(0, 0, order_plan_start_time, elapsed_time_meter.elapsed_time().count(),
               types::OrderPlanReport::Status::kFailedToBuy);
    spdlog::info("OrderPlan processing finished");
    return;
  }
  spdlog::info("Buy stage success");

  types::OrderInfo sell_order;
  types::OrderResult sell_order_result;
  sell_order.price = order_plan.min_sell_price;
  sell_order.side = types::OrderInfo::Side::SELL;
  sell_order.type = types::OrderInfo::Type::LIMIT;
  if (!ExecOrder(sell_order, order_plan.expiration_sell_ts, &sell_order_result)) {
    spdlog::warn("Failed execute SELL order");
    types::OrderInfo sell_market_order;
    sell_market_order.side = types::OrderInfo::Side::SELL;
    sell_market_order.type = types::OrderInfo::Type::MARKET;
    if (!ExecOrder(sell_market_order, 0, &sell_order_result)) {
      spdlog::error("Unexpected ExecOrder result");
      exit(-1);
    }
    SendReport(buy_order_result.price, sell_order_result.price, order_plan_start_time,
               elapsed_time_meter.elapsed_time().count(),
               types::OrderPlanReport::Status::kFailedToProfitSell);
    spdlog::info("OrderPlan processing finished");
    return;
  }
  SendReport(buy_order_result.price, sell_order_result.price, order_plan_start_time,
             elapsed_time_meter.elapsed_time().count(),
             types::OrderPlanReport::Status::kOk);
  spdlog::info("Sell stage success");

  spdlog::info("OrderPlan processing finished");
}

bool OrderPlanManager::ExecOrder(const types::OrderInfo &order,
                                 utils::Timestamp expiration_ts,
                                 types::OrderResult *result) {
  const auto now = utils::GlobalClock::Instance().Now();
  if (types::OrderInfo::Type::MARKET != order.type && now >= expiration_ts) {
    spdlog::warn("Order has expired");
    return false;
  }

  spdlog::debug("placing order: side {}, type {}", static_cast<int>(order.side),
                static_cast<int>(order.type));
  auto order_response = order_manager_->PlaceOrder(order);
  spdlog::debug("order placed");
  std::future_status order_result_status = std::future_status::ready;

  if (types::OrderInfo::Type::LIMIT == order.type) {
    auto time_left = utils::ChronoTimestampPrecision(expiration_ts - now);
    order_result_status = order_response.order_result.wait_for(time_left);
  } else {
    order_response.order_result.wait();
  }

  if (std::future_status::timeout == order_result_status) {
    spdlog::debug("order timeout, cancel it");
    order_manager_->CancelOrder(order_response.order_id.get());
    auto cancel_responce_status =
        order_response.order_result.wait_for(kMaxCancelResponceTime);
    *result = order_response.order_result.get();
    if (cancel_responce_status == std::future_status::timeout) {
      spdlog::error("Unexpected status or cancel timeout");
      exit(-1);
    }

    if (result->status != types::OrderResult::Status::kCanceled) {
      spdlog::warn("Unexpected status for cancelation");
    }
    spdlog::debug("order canceled");
    return false;
  }
  *result = order_response.order_result.get();
  return true;
}

void OrderPlanManager::SendReport(market_stream::types::DoubleType buy_price,
                                  market_stream::types::DoubleType sold_price,
                                  utils::Timestamp start_time_ms,
                                  utils::TimestampPrecision duration_ms,
                                  types::OrderPlanReport::Status status) {
  types::OrderPlanReport report;
  report.buy_price = buy_price;
  report.sold_price = sold_price;
  report.start_ts = start_time_ms;
  report.duration = duration_ms;
  report.status = status;
  benchmark_data_collector_->AddOrderPlanReport(report);
}

void OrderPlanManager::OrderPlansProcessThread() {
  spdlog::info("OrderPlansProcessThread started");
  while (!is_stopped_) {
    std::unique_lock<std::mutex> lock(mutex_);
    cv_.wait(lock, [this] { return order_plan_info_ != nullptr || is_stopped_; });

    if (is_stopped_) {
      break;
    }

    lock.unlock();

    try {
      ProcessOrderPlan(*order_plan_info_);
    } catch (const std::exception &e) {
      spdlog::error("An exception cought during plan processing: {}", e.what());
    }

    order_plan_info_ = nullptr;
  }
  spdlog::info("OrderPlansProcessThread finished");
}
}  // namespace analyzer