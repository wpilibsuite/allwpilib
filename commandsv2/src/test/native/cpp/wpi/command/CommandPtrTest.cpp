// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include "CommandTestBase.hpp"
#include "wpi/commands2/CommandPtr.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/system/Errors.hpp"

using namespace wpi::cmd;
class CommandPtrTest : public CommandTestBase {};

TEST_F(CommandPtrTest, MovedFrom) {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  CommandPtr movedFrom = cmd::Run([&counter] { counter++; });
  CommandPtr movedTo = std::move(movedFrom);

  EXPECT_NO_FATAL_FAILURE(scheduler.Schedule(movedTo));
  EXPECT_NO_FATAL_FAILURE(scheduler.Run());

  EXPECT_EQ(1, counter);
  EXPECT_NO_FATAL_FAILURE(scheduler.Cancel(movedTo));

  EXPECT_THROW(scheduler.Schedule(movedFrom), wpi::RuntimeError);
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
  EXPECT_THROW(movedFrom.IsScheduled(), wpi::RuntimeError);
  EXPECT_THROW(static_cast<void>(std::move(movedFrom).Repeatedly()),
               wpi::RuntimeError);

  EXPECT_EQ(1, counter);
}

TEST_F(CommandPtrTest, NullInitialization) {
  EXPECT_THROW(auto cmd = CommandPtr{std::unique_ptr<Command>{}},
               wpi::RuntimeError);
}
