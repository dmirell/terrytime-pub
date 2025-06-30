#ifndef INCLUDE_UTILS_TIME_GLOBAL_CLOCK_H_
#define INCLUDE_UTILS_TIME_GLOBAL_CLOCK_H_

#include <cctype>
#include <memory>

#include "utils/time/i_time_provider.h"

namespace utils {

class GlobalClock : public ITimeProvider {
 public:
  GlobalClock(const GlobalClock &) = delete;
  GlobalClock(GlobalClock &&) = delete;
  GlobalClock &operator=(const GlobalClock &) = delete;
  GlobalClock &operator=(GlobalClock &&) = delete;

  void SetTimeProvider(const std::shared_ptr<ITimeProvider> &time_provider);
  static GlobalClock &Instance();

  // ITimeProvider
  virtual void WaitUntil(Timestamp timestamp) override;
  virtual Timestamp Now() const override;

 protected:
  GlobalClock();

  std::shared_ptr<ITimeProvider> time_provider_;
};

}  // namespace utils

#endif  // INCLUDE_UTILS_TIME_GLOBAL_CLOCK_H_