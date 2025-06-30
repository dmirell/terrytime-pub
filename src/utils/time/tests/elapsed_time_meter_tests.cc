#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#include "utils/time/elapsed_time_meter.h"

TEST(ElapsedTimeMeter, GivenElapsedTimeMeter_When1SecPass_ThenElapsesTimeAtLeast1Sec) {
  // Given
  utils::ElapsedTimeMeter elapsed_time_meter;

  // When
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  // Then
  auto elapsed = elapsed_time_meter.elapsed_time();
  EXPECT_GE(elapsed.count(), 1000);
  EXPECT_LE(elapsed.count(), 1100);
}