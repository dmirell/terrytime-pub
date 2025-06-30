#include <gtest/gtest.h>

#include "commands/command_orderbook_test_handler.h"

TEST(CommandOrderBookTestHandler,
     GivenWrongCommandLineText_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 2;
  const char* argv[] = {"stream orderbook-handle-check",
                        "--savd-stea-path=\"/test/path/\""};

  // Then
  EXPECT_EXIT(commands::CommandOrderBookTestHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(CommandOrderBookTestHandler,
     GivenNoOptCommandLine_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 1;
  const char* argv[] = {"stream orderbook-handle-check"};

  // Then
  EXPECT_EXIT(commands::CommandOrderBookTestHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}
