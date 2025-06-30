#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <cctype>
#include <chrono>
#include <thread>
#include <vector>

#include "utils/tests/helpers/scoped_logger.h"
#include "utils/time/mock_time_provider.h"

TEST(MockTimeProvider, GivenStartTime1234_WhenWaitUntil2234_ThenTimeAt2234) {
  // Given
  utils::MockTimeProvider mock_time_provider(1234);

  // When
  mock_time_provider.WaitUntil(2234);

  // Then
  auto now = mock_time_provider.Now();
  EXPECT_GE(now, 2234);
  EXPECT_LE(now, 2334);
}

TEST(MockTimeProvider, GivenAwaitingForFarTime_WhenJump_ThenTimeMoved) {
  // Given
  utils::ScopedLogger logger(spdlog::level::warn);

  std::vector<uint64_t> timestamps = {1234, 1234567810, 2234567810, 3234567810};
  utils::MockTimeProvider mock_time_provider(timestamps[0]);

  for (int i = 1; i < timestamps.size(); i++) {
    std::thread t([&]() { mock_time_provider.WaitUntil(timestamps[i]); });
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto now = mock_time_provider.Now();
    EXPECT_LE(now, timestamps[i]);

    // When
    mock_time_provider.JumpToFirstAwaitingTimestamp();

    auto logs = logger.str();
    ASSERT_EQ(logs.find("no awaiting time to jump"), std::string::npos);

    // Then
    now = mock_time_provider.Now();
    EXPECT_GE(now, timestamps[i] - 50);
    EXPECT_LE(now, timestamps[i] + 50);

    t.join();
  }
}

TEST(MockTimeProvider, GivenAwaitingForFarTime_WhenJumpToTime_ThenTimeMovedToTs) {
  // Given
  utils::ScopedLogger logger(spdlog::level::warn);

  std::vector<uint64_t> timestamps = {1234, 1234567810, 2234567810, 3234567810};
  utils::MockTimeProvider mock_time_provider(timestamps[0]);

  for (int i = 1; i < timestamps.size(); i++) {
    std::thread t([&]() { mock_time_provider.WaitUntil(timestamps[i]); });
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto now = mock_time_provider.Now();
    EXPECT_LE(now, timestamps[i]);

    // When
    mock_time_provider.JumpToTime(timestamps[i]);

    auto logs = logger.str();
    ASSERT_EQ(logs.find("no awaiting time to jump"), std::string::npos);

    // Then
    now = mock_time_provider.Now();
    EXPECT_GE(now, timestamps[i] - 50);
    EXPECT_LE(now, timestamps[i] + 50);

    t.join();
  }
}
