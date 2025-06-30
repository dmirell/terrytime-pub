#include "analyzer/order_book_snapshot_provider.h"

#include <spdlog/spdlog.h>

#include <sstream>

#include "analyzer/scoped_unit_state.h"
#include "utils/helpers.h"
#include "utils/time/global_clock.h"
#include "utils/time/types.h"

namespace analyzer {

namespace {
const std::size_t gMaxOrderBookLevelsHandleCount = 6000;
const utils::TimestampPrecision gSnapshotLifeTime = 100;
}  // namespace

OrderBookSnapshotProvider::OrderBookSnapshotProvider(
    const std::weak_ptr<MQMSEventHubHandler> &event_handler,
    const std::weak_ptr<MQOSEventHubDispatcher> &dispatcher,
    const std::shared_ptr<OrderBookSnapshotProviderUnitState> &unit_state)
    : dispatcher_(dispatcher), unit_state_(unit_state) {
  SUBSCRIBE_TO_EVENT(event_handler, &OrderBookSnapshotProvider::OnMarketStreamEvent);
}

market_stream::types::OrderBook OrderBookSnapshotProvider::GetSnapshot(
    utils::Timestamp last_update_ts) {
  std::unique_lock<std::mutex> lock(mutex_);
  cv_.wait(lock, [this, last_update_ts]() {
    return last_update_ts <= order_book_.received_timestamp;
  });
  return order_book_;
}

void OrderBookSnapshotProvider::OnMarketStreamEvent(MQMarketStream::Event event,
                                                    const void *data) {
  if (MQMarketStream::Event::kOrderBookUpdateEvent == event) {
    if (nullptr != unit_state_) {
      unit_state_->SetBusy();
    }
    auto order_book_update = static_cast<const market_stream::types::OrderBook *>(data);
    OnOrderBookUpdateEvent(*order_book_update);

    if (nullptr != unit_state_) {
      unit_state_->SetReady();
    }
  }
}

void OrderBookSnapshotProvider::OnOrderBookUpdateEvent(
    market_stream::types::OrderBook update) {
  std::lock_guard<std::mutex> lock(mutex_);

  // PrepareItems(&(update.bids), false);
  // PrepareItems(&(update.asks), true);

  order_book_.timestamp = update.timestamp;
  order_book_.received_timestamp = update.received_timestamp;
  order_book_.bids = UpdateOrderBookItems(order_book_.bids, update.bids, false);
  order_book_.asks = UpdateOrderBookItems(order_book_.asks, update.asks, true);
  order_book_.bids.resize(
      std::min(gMaxOrderBookLevelsHandleCount, order_book_.bids.size()));
  order_book_.asks.resize(
      std::min(gMaxOrderBookLevelsHandleCount, order_book_.asks.size()));

  assert(CheckItemsOrder(order_book_.bids, false));
  assert(CheckItemsOrder(order_book_.asks, true));

  cv_.notify_all();
  auto dispatcher_lock = dispatcher_.lock();
  if (dispatcher_lock) {
    dispatcher_lock->DispatchEvent<MQOrderBookStream::Event::kNewSnapshotAvailable>(
        order_book_);
  }
}

void OrderBookSnapshotProvider::PrepareItems(
    market_stream::types::OrderBook::Items *items, bool is_items_order_increasing) {
  using Item = market_stream::types::OrderBook::Item;
  std::sort(items->begin(), items->end(),
            [is_items_order_increasing](const Item &l, const Item &r) {
              return is_items_order_increasing ? l.price < r.price : l.price > r.price;
            });

  market_stream::types::OrderBook::Items new_items;
  new_items.reserve(items->size());
  for (int i = 0; i < static_cast<int>(items->size()); i++) {
    new_items.push_back((*items)[i]);
    if (i > 0 && (*items)[i - 1].price == new_items.back().price) {
      if ((*items)[i - 1].quantity != new_items.back().quantity) {
        spdlog::error("Failed pairs: {}, {}, failed q-s: {}, {}", (*items)[i].price.str(),
                      (*items)[i - 1].price.str(), (*items)[i].quantity.str(),
                      (*items)[i - 1].quantity.str());
        assert(false);
      }
      new_items.pop_back();
    }
  }
  *items = std::move(new_items);
}

market_stream::types::OrderBook::Items OrderBookSnapshotProvider::UpdateOrderBookItems(
    const market_stream::types::OrderBook::Items &old_items,
    const market_stream::types::OrderBook::Items &update,
    bool is_items_order_increasing) {
  market_stream::types::OrderBook::Items new_items;
  new_items.reserve(old_items.size());

  auto old_items_it = old_items.begin();
  for (const auto &it : update) {
    bool is_found_place_to_insert = false;
    while (old_items_it < old_items.end()) {
      is_found_place_to_insert = is_items_order_increasing
                                     ? old_items_it->price > it.price
                                     : old_items_it->price < it.price;
      is_found_place_to_insert = is_found_place_to_insert ||
                                 utils::IsDoublesEqual(old_items_it->price, it.price);
      if (is_found_place_to_insert) {
        if (!utils::IsDoublesEqual(it.quantity, 0)) {
          new_items.push_back(it);
        }
        if (utils::IsDoublesEqual(old_items_it->price, it.price)) {
          ++old_items_it;
        }
        break;
      } else {
        new_items.push_back(*old_items_it);
      }
      ++old_items_it;
    }
    if (!is_found_place_to_insert) {
      new_items.push_back(it);
    }
  }
  for (; old_items_it < old_items.end(); ++old_items_it) {
    new_items.push_back(*old_items_it);
  }
  return new_items;
}

bool OrderBookSnapshotProvider::CheckItemsOrder(
    const market_stream::types::OrderBook::Items &items, bool is_items_order_increasing) {
  for (int i = 0; i < static_cast<int>(items.size()) - 1; i++) {
    bool is_pass = is_items_order_increasing ? items[i].price < items[i + 1].price
                                             : items[i].price > items[i + 1].price;
    is_pass = is_pass && !utils::IsDoublesEqual(items[i].price, items[i + 1].price);
    if (!is_pass) {
      std::stringstream ss;
      ss << items;
      spdlog::error("Failed pairs: {}, {}, failed q-s: {}, {}", items[i].price.str(),
                    items[i + 1].price.str(), items[i].quantity.str(),
                    items[i + 1].quantity.str());
      return false;
    }
  }
  return true;
}

}  // namespace analyzer