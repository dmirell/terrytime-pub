#ifndef INCLUDE_ANALYZER_BENCHMARK_MANAGER_H_
#define INCLUDE_ANALYZER_BENCHMARK_MANAGER_H_

#include <array>
#include <atomic>
#include <memory>
#include <thread>

#include "market_stream/binapi_client.h"
#include "market_stream/types/types.h"

namespace analyzer {

class OrderBookSnapshotProvider;

class BenchmarkManager {
 public:
  BenchmarkManager() = delete;
  BenchmarkManager(const BenchmarkManager &) = delete;
  BenchmarkManager(BenchmarkManager &&) = delete;
  BenchmarkManager &operator=(const BenchmarkManager &) = delete;
  BenchmarkManager &operator=(BenchmarkManager &&) = delete;

  BenchmarkManager(
      const std::string &symbol,
      const std::shared_ptr<OrderBookSnapshotProvider> &order_book_snap_provider);
  ~BenchmarkManager() = default;

  void Run();

 private:
  struct BenchmarkOrderBookEquality {
    static const std::size_t kLevelsCountsSize = 4;
    static const std::array<int, kLevelsCountsSize> kLevelsCounts;
    std::array<market_stream::types::DoubleType, kLevelsCountsSize> bids_eq{-1, -1, -1,
                                                                            -1};
    std::array<market_stream::types::DoubleType, kLevelsCountsSize> asks_eq{-1, -1, -1,
                                                                            -1};
  };

  void OrderBookHandleCheckThread();
  BenchmarkOrderBookEquality EstimateOrderBookHandle(
      const market_stream::types::OrderBook &local_ob,
      const market_stream::types::OrderBook &server_ob) const;
  market_stream::types::DoubleType EstimateOrderBookItemsLevels(
      const market_stream::types::OrderBook::Items &local_items,
      const market_stream::types::OrderBook::Items &server_items, int levels_count,
      bool is_increasing) const;
  void PrintBenchmarkOrderBookEquality(const BenchmarkOrderBookEquality &equality) const;

  std::atomic<bool> threads_stopped_{false};
  std::thread orderbook_handle_check_thread_;
  market_stream::BinAPIClient binapi_client_;
  std::shared_ptr<OrderBookSnapshotProvider> order_book_snap_provider_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_BENCHMARK_MANAGER_H_