#include "utils/time/global_clock.h"

#include "utils/time/time_provider.h"

namespace utils {

GlobalClock::GlobalClock() : time_provider_(std::make_shared<TimeProvider>()) {}

void GlobalClock::SetTimeProvider(const std::shared_ptr<ITimeProvider> &time_provider) {
  time_provider_ = time_provider;
}

GlobalClock &GlobalClock::Instance() {
  static GlobalClock instance;
  return instance;
}

void GlobalClock::WaitUntil(Timestamp timestamp) { time_provider_->WaitUntil(timestamp); }

Timestamp GlobalClock::Now() const { return time_provider_->Now(); }

}  // namespace utils
