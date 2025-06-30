#ifndef INCLUDE_UTILS_ELAPSED_TIME_METER_H_
#define INCLUDE_UTILS_ELAPSED_TIME_METER_H_

#include <cctype>
#include <chrono>

namespace utils {

class ElapsedTimeMeter {
  using ClockType = std::chrono::steady_clock;
  using TimePoint = std::chrono::time_point<ClockType>;

 public:
  using Duration = std::chrono::milliseconds;

  ElapsedTimeMeter();
  ElapsedTimeMeter(const ElapsedTimeMeter &) = default;
  ElapsedTimeMeter(ElapsedTimeMeter &&) = default;
  ElapsedTimeMeter &operator=(const ElapsedTimeMeter &) = default;
  ElapsedTimeMeter &operator=(ElapsedTimeMeter &&) = default;

  Duration elapsed_time() const;

 private:
  TimePoint start_time_;
};

}  // namespace utils

#endif  // INCLUDE_UTILS_ELAPSED_TIME_METER_H_