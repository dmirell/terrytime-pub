#include "analyzer/benchmark_orchestrator.h"

#include <spdlog/spdlog.h>

#include <chrono>
#include <functional>
#include <sstream>

#include "analyzer/observable_units.h"
#include "market_stream/saved_market_stream_forwarder.h"
#include "utils/time/global_clock.h"
#include "utils/time/mock_time_provider.h"

namespace analyzer {

BenchmarkOrchestrator::BenchmarkOrchestrator(
    const std::shared_ptr<market_stream::SavedMarketStreamForwarder> &stream_forwarder,
    bool enable_ts_jump)
    : is_stopped_(false),
      is_enabled_ts_jump_(enable_ts_jump),
      stream_forwarder_(stream_forwarder) {
  ObservableUnits::Instance().SubscribeToAllUnitsReady(
      std::bind(&BenchmarkOrchestrator::OnAllUnitsReady, this));
}

BenchmarkOrchestrator::~BenchmarkOrchestrator() {
  ObservableUnits::Instance().Unsubscribe();
}

void BenchmarkOrchestrator::Go() {
  while (!is_stopped_) {
    spdlog::info("read next");
    utils::Timestamp next_data_ts;
    if (!stream_forwarder_->ReadNext(&next_data_ts)) {
      spdlog::info("Data read finished");
      break;
    }

    if (nullptr == mock_time_provider_) {
      mock_time_provider_ = std::make_shared<utils::MockTimeProvider>(next_data_ts);
      utils::GlobalClock::Instance().SetTimeProvider(mock_time_provider_);
    }

    spdlog::debug("current ts: {}, next data ts: {}",
                  utils::GlobalClock::Instance().Now(), next_data_ts);
    if (is_enabled_ts_jump_ && ObservableUnits::Instance().GetBusyUnits().empty()) {
      mock_time_provider_->JumpToTime(next_data_ts);
    } else {
      utils::GlobalClock::Instance().WaitUntil(next_data_ts);
    }

    if (is_stopped_) {
      break;
    }

    const auto busy_units = ObservableUnits::Instance().GetBusyUnits();
    if (!busy_units.empty()) {
      std::stringstream ss;
      ss << busy_units;
      spdlog::warn("forwarding event while some of units is not ready: {}", ss.str());
    }

    stream_forwarder_->ForwardNext();
  }
  spdlog::warn("BenchmarkOrchestrator run finished");
}

void BenchmarkOrchestrator::Stop() { is_stopped_ = true; }

void BenchmarkOrchestrator::OnAllUnitsReady() {
  spdlog::info("OnAllUnitsReady");
  if (is_enabled_ts_jump_ && nullptr != mock_time_provider_) {
    mock_time_provider_->JumpToFirstAwaitingTimestamp();
  }
}

}  // namespace analyzer
