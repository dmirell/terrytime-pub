#include "analyzer/benchmark_manager.h"

#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>

#include "analyzer/order_book_snapshot_provider.h"
#include "utils/helpers.h"

namespace analyzer {

namespace {
const auto gOrderBookCheckPeriod = std::chrono::minutes(5);
}
const std::array<int, BenchmarkManager::BenchmarkOrderBookEquality::kLevelsCountsSize>
    BenchmarkManager::BenchmarkOrderBookEquality::kLevelsCounts = {50, 100, 1000, 4000};

BenchmarkManager::BenchmarkManager(
    const std::string &symbol,
    const std::shared_ptr<OrderBookSnapshotProvider> &order_book_snap_provider)
    : binapi_client_(symbol), order_book_snap_provider_(order_book_snap_provider) {}

void BenchmarkManager::Run() {
  binapi_client_.Run();
  orderbook_handle_check_thread_ =
      std::thread(std::bind(&BenchmarkManager::OrderBookHandleCheckThread, this));
}

void BenchmarkManager::OrderBookHandleCheckThread() {
  while (!threads_stopped_) {
    std::this_thread::sleep_for(gOrderBookCheckPeriod);
    binapi_client_.GetDepthAsync([this](binapi::rest::depths_t &&depths) {
      spdlog::debug("depth diagram recieved.");
      auto result =
          EstimateOrderBookHandle(order_book_snap_provider_->GetSnapshot(),
                                  market_stream::types::OrderBook(std::move(depths)));
      PrintBenchmarkOrderBookEquality(result);
    });
  }
}

BenchmarkManager::BenchmarkOrderBookEquality BenchmarkManager::EstimateOrderBookHandle(
    const market_stream::types::OrderBook &local_ob,
    const market_stream::types::OrderBook &server_ob) const {
  spdlog::debug("depth diagram benchmark process started.");
  BenchmarkOrderBookEquality result;
  const int server_ob_min_size = std::min(server_ob.asks.size(), server_ob.bids.size());
  for (int i = 0; i < BenchmarkOrderBookEquality::kLevelsCountsSize; i++) {
    const int current_levels_count = BenchmarkOrderBookEquality::kLevelsCounts[i];
    if (server_ob_min_size < current_levels_count) {
      break;
    }
    result.bids_eq[i] = EstimateOrderBookItemsLevels(local_ob.bids, server_ob.bids,
                                                     current_levels_count, false);
    result.asks_eq[i] = EstimateOrderBookItemsLevels(local_ob.asks, server_ob.asks,
                                                     current_levels_count, true);
  }
  return result;
}

market_stream::types::DoubleType BenchmarkManager::EstimateOrderBookItemsLevels(
    const market_stream::types::OrderBook::Items &local_items,
    const market_stream::types::OrderBook::Items &server_items, int levels_count,
    bool is_increasing) const {
  auto local_items_copy = local_items;
  auto server_items_copy = server_items;
  if (!is_increasing) {
    std::reverse(local_items_copy.begin(), local_items_copy.end());
    std::reverse(server_items_copy.begin(), server_items_copy.end());
  }
  market_stream::types::DoubleType result = -1, dot_result = 0.0, server_vec_length = 0.0,
                                   local_vec_length = 0.0;
  int j = 0;
  for (int i = 0; i < levels_count; i++) {
    const auto &server_item = server_items_copy[i];
    server_vec_length += server_item.quantity * server_item.quantity;
    for (; j < local_items_copy.size(); j++) {
      const auto &local_item = local_items_copy[j];
      if (!is_increasing) {
        //  spdlog::info("local_item={}, server_item={}", local_item.price,
        //            server_item.price);
      }
      if (utils::IsDoublesEqual(local_item.price, server_item.price)) {
        dot_result += local_item.quantity * server_item.quantity;
        local_vec_length += local_item.quantity * local_item.quantity;
        break;
      }
      if (local_item.price > server_item.price) {
        break;
      }
    }
  }
  server_vec_length = std::sqrt(server_vec_length.convert_to<double>());
  local_vec_length = std::sqrt(local_vec_length.convert_to<double>());
  /* spdlog::info("dot_result={}, server_vec_length={}, local_vec_length={}", dot_result,
               server_vec_length, local_vec_length); */
  if (server_vec_length != 0 && local_vec_length != 0) {
    result = dot_result / (server_vec_length * local_vec_length);
  } else {
    spdlog::error(
        "Benchmark error: server_vec_length={}, local_vec_length={}, is_increasing={}",
        server_vec_length.str(), local_vec_length.str(), is_increasing);
  }
  result = (result + 1) / 2 * 100;
  return result;
}

void BenchmarkManager::PrintBenchmarkOrderBookEquality(
    const BenchmarkOrderBookEquality &equality) const {
  assert(equality.bids_eq.size() == equality.asks_eq.size());
  std::cout << "OrderBook handle check for first ";
  for (int i = 0; i < BenchmarkOrderBookEquality::kLevelsCountsSize; i++) {
    std::cout << BenchmarkOrderBookEquality::kLevelsCounts[i];
    if (i + 1 != BenchmarkOrderBookEquality::kLevelsCountsSize) {
      std::cout << ", ";
    }
  }
  std::cout << " levels (b_eq, a_eq): ";
  std::cout << std::fixed << std::setprecision(2);
  for (int i = 0; i < BenchmarkOrderBookEquality::kLevelsCountsSize; i++) {
    std::cout << "(" << equality.bids_eq[i] << "%, " << equality.asks_eq[i] << "%)";
    if (i + 1 != BenchmarkOrderBookEquality::kLevelsCountsSize) {
      std::cout << ", ";
    }
  }
  std::cout << "." << std::endl;
}
}  // namespace analyzer