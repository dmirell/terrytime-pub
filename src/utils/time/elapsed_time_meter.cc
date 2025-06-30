#include "utils/time/elapsed_time_meter.h"

namespace utils {

ElapsedTimeMeter::ElapsedTimeMeter() : start_time_(ClockType::now()) {}

ElapsedTimeMeter::Duration ElapsedTimeMeter::elapsed_time() const {
  TimePoint current_time = ClockType::now();
  Duration elapsed_time =
      std::chrono::duration_cast<Duration>(current_time - start_time_);
  return elapsed_time;
}
}  // namespace utils