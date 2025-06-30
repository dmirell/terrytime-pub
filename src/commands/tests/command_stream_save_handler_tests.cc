#include <gtest/gtest.h>

#include "commands/command_stream_save_handler.h"

TEST(CommandStreamSaveHandler,
     GivenWrongCommandLineText_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 2;
  const char* argv[] = {"streamsave", "--save-path=\"/test/path/\""};

  // Then
  EXPECT_EXIT(commands::CommandStreamSaveHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}

TEST(CommandStreamSaveHandler,
     GivenNoOptCommandLine_WhenCreateHandler_ThenTheProgramExit) {
  // Given
  int argc = 1;
  const char* argv[] = {"streamsave"};

  // Then
  EXPECT_EXIT(commands::CommandStreamSaveHandler(argc, argv),
              ::testing::ExitedWithCode(EXIT_FAILURE), "");
}
