#include <gmock/gmock.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "analyzer/benchmark_data_collector.h"
#include "utils/tests/helpers/scoped_logger.h"

namespace analyzer {
namespace {

void SendOrderPlanReports(BenchmarkDataCollector& data_collector) {
  std::vector<types::OrderPlanReport> reports;
  types::OrderPlanReport report;

  report.buy_price = 1.2;
  report.sold_price = 1.4;
  report.duration = 100;
  report.start_ts = 0;
  report.status = types::OrderPlanReport::Status::kOk;
  reports.push_back(report);

  report.buy_price = 1.2;
  report.sold_price = 1.9;
  report.duration = 200;
  report.start_ts = 60 * 60 * 1000;
  report.status = types::OrderPlanReport::Status::kOk;
  reports.push_back(report);

  report.buy_price = 1.2;
  report.sold_price = 1.6;
  report.duration = 300;
  report.start_ts = 36 * 60 * 60 * 1000;
  report.status = types::OrderPlanReport::Status::kOk;
  reports.push_back(report);

  report.buy_price = 1.2;
  report.sold_price = 1.0;
  report.duration = 120;
  report.start_ts = 1000;
  report.status = types::OrderPlanReport::Status::kFailedToProfitSell;
  reports.push_back(report);

  report.buy_price = 1.2;
  report.sold_price = 0.5;
  report.duration = 1234;
  report.start_ts = 2000;
  report.status = types::OrderPlanReport::Status::kFailedToProfitSell;
  reports.push_back(report);

  report.buy_price = 0;
  report.sold_price = 0;
  report.duration = 200;
  report.start_ts = 22222;
  report.status = types::OrderPlanReport::Status::kFailedToBuy;
  reports.push_back(report);

  report.buy_price = 0;
  report.sold_price = 0;
  report.duration = 200;
  report.start_ts = 222222;
  report.status = types::OrderPlanReport::Status::kPlanExpired;
  reports.push_back(report);

  report.buy_price = 0;
  report.sold_price = 0;
  report.duration = 200;
  report.start_ts = 322222;
  report.status = types::OrderPlanReport::Status::kPlanExpired;
  reports.push_back(report);

  for (const auto& it : reports) {
    data_collector.AddOrderPlanReport(it);
  }
}
}  // namespace

TEST(BenchmarkDataCollector,
     GivenBenchmarkDataCollector_WhenSendReports_ThenTotalReportGeneratedCorrectly) {
  // Given
  utils::ScopedLogger logger(spdlog::level::warn);
  BenchmarkDataCollector data_collector;

  // When
  SendOrderPlanReports(data_collector);

  // Then
  const auto total_report = data_collector.GenerateTotalReport();

  EXPECT_NE(total_report.find("Average Profit Multiplier: 1.0650672667"),
            std::string::npos);
  EXPECT_NE(total_report.find("Median Profit Multiplier: 1.1649173229"),
            std::string::npos);
  EXPECT_NE(total_report.find("Average Gain Multiplier: 1.3590702101"),
            std::string::npos);
  EXPECT_NE(total_report.find("Median Gain Multiplier: 1.3313340833"), std::string::npos);
  EXPECT_NE(total_report.find("Average Lose Multiplier: 0.6240628516"),
            std::string::npos);
  EXPECT_NE(total_report.find("Median Lose Multiplier: 0.6240628516"), std::string::npos);
  EXPECT_NE(total_report.find("Average Order Plan Duration (ms): 319.250000"),
            std::string::npos);
  EXPECT_NE(total_report.find("Median Order Plan Duration (ms): 200.000000"),
            std::string::npos);
  EXPECT_NE(total_report.find(">>> status ok <<<\n"
                              // "Avarage count per day: 2.000000\n"
                              "Total count: 3"),
            std::string::npos);
  EXPECT_NE(total_report.find(">>> status failed_to_profit_sell <<<\n"
                              // "Avarage count per day: 172800.000000\n"
                              "Total count: 2"),
            std::string::npos);
  EXPECT_NE(total_report.find(">>> status failed_to_buy <<<\n"
                              // "Avarage count per day: 1.000000\n"
                              "Total count: 1"),
            std::string::npos);
  EXPECT_NE(total_report.find(">>> status plan_expired <<<\n"
                              // "Avarage count per day: 1728.000000\n"
                              "Total count: 2"),
            std::string::npos);
  EXPECT_NE(logger.str().find("only one order plan executed for status 2, not enough "
                              "data to calculate average_count_per_day for it"),
            std::string::npos);
}

TEST(BenchmarkDataCollector,
     GivenBenchmarkDataCollector_WhenSendReports_ThenTotalReportJsonGeneratedCorrectly) {
  using json = nlohmann::json;

  // Given
  utils::ScopedLogger logger(spdlog::level::warn);
  BenchmarkDataCollector data_collector;

  // When
  SendOrderPlanReports(data_collector);

  // Then
  auto total_report_json = data_collector.GenerateTotalReportJson();
  json j = json::parse(total_report_json);

  // EXPECT_EQ
  EXPECT_EQ(j["average_gain_percent"].template get<std::string>(),
            "1.359070210069444470105517");
  EXPECT_EQ(j["average_lose_percent"].template get<std::string>(),
            "0.624062851562500023094965");
  EXPECT_EQ(j["average_order_plan_duration_ms"].template get<std::string>(), "319.25");
  EXPECT_EQ(j["average_profit_percent"].template get<std::string>(),
            "1.065067266666666691301295");
  EXPECT_EQ(j["median_gain_percent"].template get<std::string>(),
            "1.331334083333333456506479");
  EXPECT_EQ(j["median_lose_percent"].template get<std::string>(),
            "0.624062851562500015625");
  EXPECT_EQ(j["median_order_plan_duration_ms"].template get<std::string>(), "200");
  EXPECT_EQ(j["median_profit_percent"].template get<std::string>(),
            "1.164917322916666635873379");

  const auto& j_status = j["status"];
  EXPECT_EQ(j_status["failed_to_buy"]["count"].template get<int>(), 1);
  EXPECT_EQ(j_status["failed_to_profit_sell"]["count"].template get<int>(), 2);
  EXPECT_EQ(j_status["ok"]["count"].template get<int>(), 3);
  EXPECT_EQ(j_status["plan_expired"]["count"].template get<int>(), 2);
}

TEST(BenchmarkDataCollector, GivenBenchmarkDataCollector_WhenNoSendReports_ThenWarning) {
  // Given
  utils::ScopedLogger logger(spdlog::level::warn);
  BenchmarkDataCollector data_collector;

  // Then
  const auto total_report = data_collector.GenerateTotalReport();
  const auto total_report_json = data_collector.GenerateTotalReportJson();

  EXPECT_EQ(total_report, "No order plan reports");
  EXPECT_NE(total_report_json.find("No order plan reports"), std::string::npos);
  EXPECT_NE(logger.str().find("No order plan reports"), std::string::npos);
}
}  // namespace analyzer