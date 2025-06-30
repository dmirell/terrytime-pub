#ifndef INCLUDE_UTILS_TIME_TYPES_H_
#define INCLUDE_UTILS_TIME_TYPES_H_

#include <cctype>
#include <chrono>

namespace utils {
using Timestamp = uint64_t;
using TimestampPrecision = Timestamp;
using ChronoTimestampPrecision = std::chrono::milliseconds;
}  // namespace utils

#endif  // INCLUDE_UTILS_TIME_TYPES_H_