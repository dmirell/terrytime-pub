#include "analyzer/benchmark_data_collector.h"

#include <spdlog/spdlog.h>

#include <cmath>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>

namespace analyzer {

namespace {
const market_stream::types::DoubleType kPlatformFee("0.00075");  // TEMPORARY HERE
}
void BenchmarkDataCollector::AddOrderPlanReport(const types::OrderPlanReport& report) {
  reports_.push_back(report);
}

std::string BenchmarkDataCollector::GenerateTotalReport() const {
  const auto total_report = GetTotalReport();

  // Dump result report to string
  std::stringstream ss;
  ss << total_report;
  const auto string_result = ss.str();

  return string_result;
}

std::string BenchmarkDataCollector::GenerateTotalReportJson() const {
  using json = nlohmann::json;
  const auto total_report = GetTotalReport();

  json result;
  if (!total_report.error_message.empty()) {
    result = {{"error", total_report.error_message}};
    return result.dump(2);
  }

  result = {
      {"average_profit_percent", total_report.average_profit_percent.str()},
      {"median_profit_percent", total_report.median_profit_percent.str()},
      {"average_gain_percent", total_report.average_gain_percent.str()},
      {"median_gain_percent", total_report.median_gain_percent.str()},
      {"average_lose_percent", total_report.average_lose_percent.str()},
      {"median_lose_percent", total_report.median_lose_percent.str()},
      {"average_order_plan_duration_ms",
       total_report.average_order_plan_duration_ms.str()},
      {"median_order_plan_duration_ms", total_report.median_order_plan_duration_ms.str()},
  };

  json statuses_json;
  for (int status = 0; status < static_cast<int>(types::OrderPlanReport::Status::COUNT);
       status++) {
    const auto status_name = types::OrderPlanReport::StatusToString(
        static_cast<types::OrderPlanReport::Status>(status));
    statuses_json[status_name] = {{"count", total_report.total_count[status]}};
  }
  result["status"] = statuses_json;

  return result.dump(2);
}

BenchmarkDataCollector::TotalReport BenchmarkDataCollector::GetTotalReport() const {
  TotalReport result;
  if (reports_.size() == 0) {
    spdlog::warn("No order plan reports");
    result.error_message = "No order plan reports";
    return result;
  }

  auto is_order_plan_was_executed = [](const types::OrderPlanReport& report) {
    return report.status != types::OrderPlanReport::Status::kPlanExpired &&
           report.status != types::OrderPlanReport::Status::kFailedToBuy;
  };

  // Calculate profits
  std::vector<types::OrderPlanReport> executed_order_plans;
  for (const auto& it : reports_) {
    if (is_order_plan_was_executed(it)) {
      executed_order_plans.push_back(it);
    }
  }

  std::vector<market_stream::types::DoubleType> profits;
  for (const auto& it : executed_order_plans) {
    if (it.buy_price <= 0) {
      spdlog::error("One of buy price <= 0");
      exit(-1);
    }
    profits.push_back(it.sold_price / it.buy_price * pow(1.0 - kPlatformFee, 2.0));
  }
  if (profits.size() > 0) {
    result.average_profit_percent =
        CalculateAverage<market_stream::types::DoubleType>(profits);
    result.median_profit_percent = CalculateMedian(profits);
  } else {
    spdlog::warn("Zero profits (executed orders)");
  }

  // Calculate gains
  std::vector<market_stream::types::DoubleType> gains;
  for (const auto& it : profits) {
    if (it >= 1.0) {
      gains.push_back(it);
    }
  }
  if (gains.size() > 0) {
    result.average_gain_percent =
        CalculateAverage<market_stream::types::DoubleType>(gains);
    result.median_gain_percent = CalculateMedian(gains);
  } else {
    spdlog::warn("Zero gains");
  }

  // Calculate looses
  std::vector<market_stream::types::DoubleType> looses;
  for (const auto& it : profits) {
    if (it < 1.0) {
      looses.push_back(it);
    }
  }
  if (looses.size() > 0) {
    result.average_lose_percent =
        CalculateAverage<market_stream::types::DoubleType>(looses);
    result.median_lose_percent = CalculateMedian(looses);
  } else {
    spdlog::warn("Zero looses");
  }

  // Calculate durations
  std::vector<utils::Timestamp> durations;
  for (const auto& it : reports_) {
    durations.push_back(it.duration);
  }
  result.average_order_plan_duration_ms = CalculateAverage<utils::Timestamp>(durations);
  result.median_order_plan_duration_ms = CalculateMedian(durations);

  // Calculate counts
  for (int status = 0; status < static_cast<int>(types::OrderPlanReport::Status::COUNT);
       status++) {
    int64_t start_time = -1, last_time = -1;
    auto& total_count = result.total_count[status];
    for (const auto& it : reports_) {
      if (it.status == static_cast<types::OrderPlanReport::Status>(status)) {
        if (start_time < 0) {
          start_time = it.start_ts;
        }
        last_time = it.start_ts;
        total_count++;
      }
    }
    result.average_count_per_day[status] = -1;
    if (last_time >= 0 && start_time >= 0) {
      const auto milliseconds_per_day = 24 * 60 * 60 * 1000;
      const auto days_count =
          static_cast<double>(last_time - start_time) / milliseconds_per_day;
      if (total_count <= 0 || days_count < 0) {
        spdlog::error(
            "Wrong math during benchmark report for status - {}, total_count - {}, "
            "days_count - {}",
            status, total_count, days_count);
        exit(-1);
      }
      if (days_count == 0) {
        spdlog::warn(
            "only one order plan executed for status {}, not enough data to calculate "
            "average_count_per_day for it",
            status);
        result.average_count_per_day[status] = total_count;
      } else {
        spdlog::debug("status: {}, total_count: {}, days_count: {}", status, total_count,
                      days_count);
        result.average_count_per_day[status] = total_count / days_count;
      }
    } else {
      spdlog::warn("No order plans with status {}", static_cast<int>(status));
    }
  }

  return result;
}

std::ostream& operator<<(std::ostream& os, const BenchmarkDataCollector::TotalReport& r) {
  if (!r.error_message.empty()) {
    os << r.error_message;
    return os;
  }

  const auto separator_1 = "------------------------------------------------";
  const auto separator_2 = "================================================";

  os << std::fixed << std::setprecision(10) << std::endl;
  os << "================= Total Report =================" << std::endl;
  os << "Average Profit Multiplier: " << r.average_profit_percent << std::endl;
  os << "Median Profit Multiplier: " << r.median_profit_percent << std::endl;
  os << separator_1 << std::endl;
  os << "Average Gain Multiplier: " << r.average_gain_percent << std::endl;
  os << "Median Gain Multiplier: " << r.median_gain_percent << std::endl;
  os << separator_1 << std::endl;
  os << "Average Lose Multiplier: " << r.average_lose_percent << std::endl;
  os << "Median Lose Multiplier: " << r.median_lose_percent << std::endl;
  os << separator_2 << std::endl;
  os << "Average Order Plan Duration (ms): " << r.average_order_plan_duration_ms
     << std::endl;
  os << "Median Order Plan Duration (ms): " << r.median_order_plan_duration_ms
     << std::endl;
  os << separator_2 << std::endl;

  for (int status = 0; status < static_cast<int>(types::OrderPlanReport::Status::COUNT);
       status++) {
    os << ">>> status "
       << types::OrderPlanReport::StatusToString(
              static_cast<types::OrderPlanReport::Status>(status))
       << " <<<" << std::endl;
    // os << "Avarage count per day: " << r.average_count_per_day[status] << std::endl;
    os << "Total count: " << r.total_count[status] << std::endl;
    if (status != static_cast<int>(types::OrderPlanReport::Status::COUNT) - 1) {
      os << separator_1 << std::endl;
    }
  }
  os << separator_2 << std::endl;
  os << "============== End of Total Report =============" << std::endl;
  return os;
}

}  // namespace analyzer