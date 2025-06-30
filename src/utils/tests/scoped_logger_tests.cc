#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <cctype>
#include <chrono>

#include "utils/tests/helpers/scoped_logger.h"

TEST(ScopedLogger, GivenScopedLogger_WhenLogMessage_ThenLogsCaptured) {
  // Given
  utils::ScopedLogger logger(spdlog::level::debug);

  // When
  spdlog::info("TEST INFO");
  spdlog::debug("TEST DEBUG");
  spdlog::warn("TEST WARN");
  spdlog::error("TEST ERROR");
  spdlog::trace("TEST TRACE");

  // Then
  auto logs = logger.str();
  EXPECT_NE(logs.find("TEST INFO"), std::string::npos);
  EXPECT_NE(logs.find("TEST DEBUG"), std::string::npos);
  EXPECT_NE(logs.find("TEST WARN"), std::string::npos);
  EXPECT_NE(logs.find("TEST ERROR"), std::string::npos);
  EXPECT_EQ(logs.find("TEST TRACE"), std::string::npos);
}