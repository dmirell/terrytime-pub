#ifndef INCLUDE_ANALYZER_BENCHMARK_DATA_COLLECTOR_H_
#define INCLUDE_ANALYZER_BENCHMARK_DATA_COLLECTOR_H_

#include <algorithm>
#include <memory>
#include <numeric>

#include "market_stream/types/types.h"
#include "types/types.h"

namespace analyzer {

class BenchmarkDataCollector {
 public:
  BenchmarkDataCollector(const BenchmarkDataCollector &) = delete;
  BenchmarkDataCollector(BenchmarkDataCollector &&) = delete;
  BenchmarkDataCollector &operator=(const BenchmarkDataCollector &) = delete;
  BenchmarkDataCollector &operator=(BenchmarkDataCollector &&) = delete;

  BenchmarkDataCollector() = default;
  virtual ~BenchmarkDataCollector() = default;

  virtual void AddOrderPlanReport(const types::OrderPlanReport &report);
  std::string GenerateTotalReport() const;
  std::string GenerateTotalReportJson() const;

 private:
  struct TotalReport {
    market_stream::types::DoubleType average_profit_percent{0.0};
    market_stream::types::DoubleType median_profit_percent{0.0};
    market_stream::types::DoubleType average_gain_percent{0.0};
    market_stream::types::DoubleType median_gain_percent{0.0};
    market_stream::types::DoubleType average_lose_percent{0.0};
    market_stream::types::DoubleType median_lose_percent{0.0};
    market_stream::types::DoubleType average_order_plan_duration_ms{0.0};
    market_stream::types::DoubleType median_order_plan_duration_ms{0.0};
    market_stream::types::DoubleType average_count_per_day[static_cast<std::size_t>(
        types::OrderPlanReport::Status::COUNT)]{0.0};
    int total_count[static_cast<std::size_t>(types::OrderPlanReport::Status::COUNT)]{0};

    std::string error_message;
  };
  TotalReport GetTotalReport() const;

  friend std::ostream &operator<<(std::ostream &os, const TotalReport &r);

  template <class T, class Vec = std::vector<T>>
  market_stream::types::DoubleType CalculateAverage(const Vec &v) const {
    market_stream::types::DoubleType sum = 0;
    for (const auto &it : v) {
      sum += static_cast<market_stream::types::DoubleType>(it);
    }
    return sum / static_cast<market_stream::types::DoubleType>(v.size());
  }

  template <class T>
  market_stream::types::DoubleType CalculateMedian(T v) const {
    std::sort(std::begin(v), std::end(v));
    return (v.size() % 2 == 0) ? (v[v.size() / 2 - 1] + v[v.size() / 2]) / 2
                               : v[v.size() / 2];
  }

  std::vector<types::OrderPlanReport> reports_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_BENCHMARK_DATA_COLLECTOR_H_