#ifndef INCLUDE_UTILS_HELPERS_H_
#define INCLUDE_UTILS_HELPERS_H_

#include <chrono>
#include <limits>
#include <string>

#include "market_stream/types/types.h"

namespace utils {
void RunSyncTimerWithLogging(const std::string& timer_prefix,
                             const std::chrono::duration<int>& sleep_duration);
bool IsDoublesEqual(market_stream::types::DoubleType a,
                    market_stream::types::DoubleType b,
                    market_stream::types::DoubleType eps =
                        std::numeric_limits<market_stream::types::DoubleType>::epsilon());
}  // namespace utils

#endif  // INCLUDE_UTILS_HELPERS_H_