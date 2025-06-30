#include "utils/interval_timer.h"

#include <spdlog/spdlog.h>

namespace utils {
IntervalTimer::IntervalTimer() : initialized_(false) {}

IntervalTimer::IntervalTimer(uint64_t init_time_ms, uint64_t target_time_ms)
    : init_time_ms_(init_time_ms),
      duration_(Duration(target_time_ms - init_time_ms)),
      initialized_(true),
      elapsed_time_meter_() {}

bool IntervalTimer::valid() const { return initialized_; }

uint64_t IntervalTimer::TimeLeft() const {
  if (valid()) {
    const auto elapsed_time = elapsed_time_meter_.elapsed_time();
    if (elapsed_time >= duration_) {
      return 0;
    } else {
      return static_cast<uint64_t>((duration_ - elapsed_time).count());
    }
  } else {
    spdlog::error("Timer has not been initialized");
    return 0;
  }
}

uint64_t IntervalTimer::CurrentTime() const {
  if (!valid()) {
    spdlog::error("Timer has not been initialized");
    return 0;
  }
  return static_cast<uint64_t>(
      (Duration(init_time_ms_) + elapsed_time_meter_.elapsed_time()).count());
}

}  // namespace utils