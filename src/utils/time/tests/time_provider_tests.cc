#include <gtest/gtest.h>

#include <cctype>
#include <chrono>

#include "utils/time/time_provider.h"

namespace {

uint64_t GetSystemTimestamp() {
  auto current_time = std::chrono::system_clock::now();

  // Convert the time point to milliseconds since the epoch
  auto milliseconds_since_epoch = std::chrono::duration_cast<std::chrono::milliseconds>(
                                      current_time.time_since_epoch())
                                      .count();
  return milliseconds_since_epoch;
}

}  // namespace

TEST(TimeProvider, GivenTimeProvider_WhenWaitUntilNowPlus2234_ThenTimeAtNowPlus2234) {
  // Given
  utils::TimeProvider time_provider;

  // When
  const auto system_now = GetSystemTimestamp();
  time_provider.WaitUntil(system_now + 2234);

  // Then
  auto now = time_provider.Now();
  EXPECT_GE(now, system_now + 2234);
  EXPECT_LE(now, system_now + 2334);
}

TEST(TimeProvider, GivenTimeProvider_WhenGetSystemAndTimeProviderNow_ThenNowEqual) {
  // Given
  utils::TimeProvider time_provider;

  // When
  const auto system_now = GetSystemTimestamp();
  const auto time_provider_now = time_provider.Now();

  // Then
  EXPECT_GE(system_now, time_provider_now - 50);
  EXPECT_LE(system_now, time_provider_now + 50);
}
