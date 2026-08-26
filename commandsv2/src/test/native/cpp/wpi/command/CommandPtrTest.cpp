// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/CommandPtr.hpp"

#include <utility>

#include "CommandTestBase.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/system/Errors.hpp"

using namespace wpi::cmd;
class CommandPtrTest : public CommandTestBase {};

TEST_CASE_METHOD(CommandPtrTest, "CommandPtrTest MovedFrom",
                 "[commandsv2][command]") {
  CommandScheduler scheduler = GetScheduler();

  int counter = 0;

  CommandPtr movedFrom = wpi::cmd::Run([&counter] { counter++; });
  CommandPtr movedTo = std::move(movedFrom);

  CHECK_NOTHROW(scheduler.Schedule(movedTo));
  CHECK_NOTHROW(scheduler.Run());

  CHECK(1 == counter);
  CHECK_NOTHROW(scheduler.Cancel(movedTo));

  CHECK_THROWS_AS(scheduler.Schedule(movedFrom), wpi::RuntimeError);
  // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
  CHECK_THROWS_AS(movedFrom.IsScheduled(), wpi::RuntimeError);
  CHECK_THROWS_AS(static_cast<void>(std::move(movedFrom).Repeatedly()),
                  wpi::RuntimeError);

  CHECK(1 == counter);
}

TEST_CASE_METHOD(CommandPtrTest, "CommandPtrTest NullInitialization",
                 "[commandsv2][command]") {
  CHECK_THROWS_AS([] { CommandPtr cmd{std::unique_ptr<Command>{}}; }(),
                  wpi::RuntimeError);
}
