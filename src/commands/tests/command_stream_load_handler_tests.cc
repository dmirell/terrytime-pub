#include <gtest/gtest.h>

#include "commands/command_stream_load_handler.h"

TEST(CommandStreamLoadHandler,
     GivenWrongCommandLineText_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 2;
  const char* argv[] = {"stream load", "--savd-stea-path=\"/test/path/\""};

  // Then
  EXPECT_EXIT(commands::CommandStreamLoadHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(CommandStreamLoadHandler,
     GivenNoOptCommandLine_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 1;
  const char* argv[] = {"stream load"};

  // Then
  EXPECT_EXIT(commands::CommandStreamLoadHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}
