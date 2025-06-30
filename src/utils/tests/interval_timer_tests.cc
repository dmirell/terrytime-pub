#include <gtest/gtest.h>

#include <cctype>
#include <chrono>
#include <thread>

#include "utils/interval_timer.h"

TEST(IntervalTimer, GivenIntervalTimer_WhenTimeout_ThenTimeLeftZero) {
  // Given
  const uint64_t init_time_ms = 123456789;
  const uint64_t delay_time_ms = 5000;
  const uint64_t target_time_ms = init_time_ms + delay_time_ms;
  utils::IntervalTimer timer(init_time_ms, target_time_ms);

  ASSERT_TRUE(timer.valid());

  // When
  ASSERT_GT(timer.TimeLeft(), 0);
  std::this_thread::sleep_for(std::chrono::milliseconds(delay_time_ms));

  // Then
  EXPECT_EQ(timer.TimeLeft(), 0);
}

TEST(IntervalTimer, GivenEmptyIntervalTimer_ThenTimerInvalid) {
  // Given
  utils::IntervalTimer timer;

  // Then
  EXPECT_FALSE(timer.valid());
}