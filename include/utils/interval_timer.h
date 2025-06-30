#ifndef INCLUDE_UTILS_INTERVAL_TIMER_H_
#define INCLUDE_UTILS_INTERVAL_TIMER_H_

#include <cctype>
#include <chrono>

#include "utils/time/elapsed_time_meter.h"

namespace utils {

class IntervalTimer {
 public:
  using Duration = std::chrono::milliseconds;

  IntervalTimer();
  IntervalTimer(uint64_t init_time_ms, uint64_t target_time_ms);
  IntervalTimer(const IntervalTimer &) = default;
  IntervalTimer(IntervalTimer &&) = default;
  IntervalTimer &operator=(const IntervalTimer &) = default;
  IntervalTimer &operator=(IntervalTimer &&) = default;

  bool valid() const;
  uint64_t TimeLeft() const;
  uint64_t CurrentTime() const;

 private:
  Duration duration_;
  uint64_t init_time_ms_;
  bool initialized_;
  ElapsedTimeMeter elapsed_time_meter_;
};

}  // namespace utils

#endif  // INCLUDE_UTILS_INTERVAL_TIMER_H_