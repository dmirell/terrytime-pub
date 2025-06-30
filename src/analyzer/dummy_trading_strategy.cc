#include "analyzer/dummy_trading_strategy.h"

#include "analyzer/order_book_snapshot_provider.h"
#include "analyzer/types/types.h"

namespace analyzer {

namespace {
int gOrderPlanCounter = 0;
}

DummyTradingStrategy::DummyTradingStrategy(
    const std::weak_ptr<MQAnalyzerStreamDispatcher> &dispatcher,
    const std::shared_ptr<OrderBookSnapshotProvider> &order_book_snap_provider)
    : ITradingStrategy(dispatcher), order_book_snap_provider_(order_book_snap_provider) {}

void DummyTradingStrategy::NewTrade(const market_stream::types::Trade &trade) {
  DummyMethod(trade.received_timestamp);
}

void DummyTradingStrategy::OrderBookUpdate(
    const market_stream::types::OrderBook &update) {
  DummyMethod(update.received_timestamp);
}
void DummyTradingStrategy::DummyMethod(utils::Timestamp received_timestamp) {
  if (received_timestamp % 587 == 0 && last_sent_r_ts_ != received_timestamp) {
    const auto order_book = order_book_snap_provider_->GetSnapshot(received_timestamp);
    types::OrderPlanInfo order_plan;
    order_plan.max_buy_price = order_book.bids.front().price;
    order_plan.quantity = 1;  // not used
    order_plan.min_sell_price = order_book.bids.front().price * 1.0016;
    order_plan.expiration_ts = received_timestamp + 60000;
    order_plan.expiration_buy_ts = received_timestamp + 10000;
    order_plan.expiration_sell_ts = received_timestamp + 60000;

    if (gOrderPlanCounter == 0) {
      order_plan.expiration_ts = 0;
    } else if (gOrderPlanCounter == 1) {
      order_plan.expiration_buy_ts = 0;
    }

    // To avoid spaming usually on new trade events
    last_sent_r_ts_ = received_timestamp;

    auto dispatcher_lock = dispatcher_.lock();
    if (dispatcher_lock) {
      dispatcher_lock->DispatchEvent<MQAnalyzerStream::Event::kNewOrderPlan>(order_plan);
    }
    gOrderPlanCounter++;
  }
}

}  // namespace analyzer