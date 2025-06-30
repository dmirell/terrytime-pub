#ifndef INCLUDE_UTILS_TIME_I_TIME_PROVIDER_H_
#define INCLUDE_UTILS_TIME_I_TIME_PROVIDER_H_

#include <cctype>

#include "utils/time/types.h"

namespace utils {

class ITimeProvider {
 public:
  virtual ~ITimeProvider() = default;

  virtual void WaitUntil(Timestamp timestamp) = 0;
  virtual Timestamp Now() const = 0;
};

}  // namespace utils

#endif  // INCLUDE_UTILS_TIME_I_TIME_PROVIDER_H_