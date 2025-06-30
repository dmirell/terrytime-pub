#ifndef INCLUDE_ANALYZER_BENCHMARK_ORCHESTRATOR_H_
#define INCLUDE_ANALYZER_BENCHMARK_ORCHESTRATOR_H_

#include <atomic>
#include <memory>

namespace market_stream {
class SavedMarketStreamForwarder;
}

namespace utils {
class MockTimeProvider;
}

namespace analyzer {

class RealMarketEmulator;

class BenchmarkOrchestrator {
 public:
  BenchmarkOrchestrator() = delete;
  BenchmarkOrchestrator(const BenchmarkOrchestrator &) = delete;
  BenchmarkOrchestrator(BenchmarkOrchestrator &&) = delete;
  BenchmarkOrchestrator &operator=(const BenchmarkOrchestrator &) = delete;
  BenchmarkOrchestrator &operator=(BenchmarkOrchestrator &&) = delete;

  BenchmarkOrchestrator(
      const std::shared_ptr<market_stream::SavedMarketStreamForwarder> &stream_forwarder,
      bool enable_ts_jump = true);
  virtual ~BenchmarkOrchestrator();

  void Go();
  void Stop();

 private:
  void OnAllUnitsReady();

  const bool is_enabled_ts_jump_;

  std::atomic<bool> is_stopped_;
  std::shared_ptr<utils::MockTimeProvider> mock_time_provider_;
  std::shared_ptr<market_stream::SavedMarketStreamForwarder> stream_forwarder_;
};

}  // namespace analyzer

#endif  // INCLUDE_ANALYZER_BENCHMARK_ORCHESTRATOR_H_