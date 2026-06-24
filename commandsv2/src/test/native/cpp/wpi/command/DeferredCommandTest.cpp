// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/DeferredCommand.hpp"

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"

using namespace wpi::cmd;

namespace {

void CheckDeferredFunctions(bool expectedInterrupted) {
  CommandTestBase testBase;

  int initializeCount = 0;
  int executeCount = 0;
  int isFinishedCount = 0;
  int endCount = 0;
  bool finished = false;

  DeferredCommand deferred{[&] {
                             return FunctionalCommand{
                                 [&] { initializeCount++; },
                                 [&] { executeCount++; },
                                 [&](bool interrupted) {
                                   CHECK(interrupted == expectedInterrupted);
                                   endCount++;
                                 },
                                 [&] {
                                   isFinishedCount++;
                                   return finished;
                                 }}
                                 .ToPtr();
                           },
                           {}};

  deferred.Initialize();
  CHECK(1 == initializeCount);
  deferred.Execute();
  CHECK(1 == executeCount);
  CHECK_FALSE(deferred.IsFinished());
  CHECK(1 == isFinishedCount);
  finished = true;
  CHECK(deferred.IsFinished());
  CHECK(2 == isFinishedCount);
  deferred.End(expectedInterrupted);
  CHECK(1 == endCount);
}

}  // namespace

TEST_CASE("DeferredCommandTest DeferredFunctionsInterrupted",
          "[commandsv2][command]") {
  CheckDeferredFunctions(true);
}

TEST_CASE("DeferredCommandTest DeferredFunctionsFinished",
          "[commandsv2][command]") {
  CheckDeferredFunctions(false);
}

TEST_CASE("DeferredCommandTest DeferredSupplierOnlyCalledDuringInit",
          "[commandsv2][command]") {
  int count = 0;
  DeferredCommand command{[&count] {
                            count++;
                            return None();
                          },
                          {}};

  CHECK(0 == count);
  command.Initialize();
  CHECK(1 == count);
  command.Execute();
  command.IsFinished();
  command.End(false);
  CHECK(1 == count);
}

TEST_CASE("DeferredCommandTest DeferredRequirements", "[commandsv2][command]") {
  TestSubsystem subsystem;
  DeferredCommand command{None, {&subsystem}};

  CHECK(command.GetRequirements().contains(&subsystem));
}
