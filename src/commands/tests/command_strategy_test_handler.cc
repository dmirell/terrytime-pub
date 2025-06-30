#include "commands/command_strategy_test_handler.h"

#include <gtest/gtest.h>

TEST(CommandStrategyTestHandler,
     GivenWrongCommandLineText_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 2;
  const char* argv[] = {"dummycmd", "--save-path=\"/test/path/\""};

  // Then
  EXPECT_EXIT(commands::CommandStrategyTestHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(CommandStrategyTestHandler,
     GivenNoOptCommandLine_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 1;
  const char* argv[] = {"strat test"};

  // Then
  EXPECT_EXIT(commands::CommandStrategyTestHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}
