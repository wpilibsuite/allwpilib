// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/Errors.h>

#include "CommandTestBase.h"
#include "frc/command2/CommandPtr.h"
#include "frc/command2/CommandScheduler.h"
#include "frc/command2/Commands.h"

class CommandPtrTest : public CommandTestBase {};

TEST_F(CommandPtrTest, MovedFrom) {
  frc::CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  frc::CommandPtr movedFrom = frc::cmd::Run([&counter] { counter++; });
  frc::CommandPtr movedTo = std::move(movedFrom);

  EXPECT_NO_FATAL_FAILURE(scheduler.Schedule(movedTo));
  EXPECT_NO_FATAL_FAILURE(scheduler.Run());

  EXPECT_EQ(1, counter);
  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(movedTo));

  EXPECT_THROW(scheduler.Schedule(movedFrom), frc::RuntimeError);
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
  EXPECT_THROW(movedFrom.IsScheduled(), frc::RuntimeError);
  EXPECT_THROW(static_cast<void>(std::move(movedFrom).Repeatedly()),
               frc::RuntimeError);

  EXPECT_EQ(1, counter);
}
