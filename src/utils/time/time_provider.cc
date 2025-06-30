#include "utils/time/time_provider.h"

#include <chrono>
#include <thread>

namespace utils {

void TimeProvider::WaitUntil(Timestamp timestamp) {
  auto target_time =
      std::chrono::system_clock::time_point(utils::ChronoTimestampPrecision(timestamp));
  std::this_thread::sleep_until(target_time);
}

Timestamp TimeProvider::Now() const {
  auto current_time = std::chrono::system_clock::now();
  auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                                      current_time.time_since_epoch())
                                      .count();
  return milliseconds_since_epoch;
}

}  // namespace utils
