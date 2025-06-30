#include "utils/time/mock_time_provider.h"

#include <spdlog/spdlog.h>

#include <cassert>

namespace utils {

MockTimeProvider::MockTimeProvider(Timestamp initial_time) {
  StartFromTime(initial_time);
}

void MockTimeProvider::WaitUntil(Timestamp timestamp) {
  std::unique_lock<std::mutex> lock(mutex_);
  waiting_times_.insert(timestamp);
  for (auto now = Now(); now < timestamp; now = Now()) {
    cv_.wait_for(lock, ChronoTimestampPrecision(timestamp - now));
  }
  waiting_times_.erase(timestamp);
}

Timestamp MockTimeProvider::Now() const {
  return start_time_ + elapsed_time_meter_.elapsed_time().count();
}

void MockTimeProvider::JumpToFirstAwaitingTimestamp() {
  std::lock_guard<std::mutex> lock(mutex_);

  // Find the earliest wake-up time among all threads
  if (!waiting_times_.empty()) {
    StartFromTime(*waiting_times_.begin());
    cv_.notify_all();
  } else {
    spdlog::warn("no awaiting time to jump");
  }
}

void MockTimeProvider::JumpToTime(Timestamp ts) {
  std::lock_guard<std::mutex> lock(mutex_);

  StartFromTime(ts);
  cv_.notify_all();
}

void MockTimeProvider::StartFromTime(Timestamp initial_time) {
  start_time_ = initial_time;
  elapsed_time_meter_ = ElapsedTimeMeter();
}

}  // namespace utils
