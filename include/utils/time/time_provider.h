#ifndef INCLUDE_UTILS_TIME_TIME_PROVIDER_H_
#define INCLUDE_UTILS_TIME_TIME_PROVIDER_H_

#include "utils/time/i_time_provider.h"

namespace utils {

class TimeProvider : public ITimeProvider {
 public:
  void WaitUntil(Timestamp timestamp) override;
  Timestamp Now() const override;
};

}  // namespace utils

#endif  // INCLUDE_UTILS_TIME_TIME_PROVIDER_H_