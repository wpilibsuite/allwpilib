// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/commands2/DeferredCommand.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"

using namespace frc2;

class DeferredFunctionsTest : public CommandTestBaseWithParam<bool> {};

TEST_P(DeferredFunctionsTest, DeferredFunctions) {
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
                                   EXPECT_EQ(interrupted, GetParam());
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
  EXPECT_EQ(1, initializeCount);
  deferred.Execute();
  EXPECT_EQ(1, executeCount);
  EXPECT_FALSE(deferred.IsFinished());
  EXPECT_EQ(1, isFinishedCount);
  finished = true;
  EXPECT_TRUE(deferred.IsFinished());
  EXPECT_EQ(2, isFinishedCount);
  deferred.End(GetParam());
  EXPECT_EQ(1, endCount);
}

INSTANTIATE_TEST_SUITE_P(DeferredCommandTests, DeferredFunctionsTest,
                         testing::Values(true, false));

TEST(DeferredCommandTest, DeferredSupplierOnlyCalledDuringInit) {
  int count = 0;
  DeferredCommand command{[&count] {
                            count++;
                            return cmd::None();
                          },
                          {}};

  EXPECT_EQ(0, count);
  command.Initialize();
  EXPECT_EQ(1, count);
  command.Execute();
  command.IsFinished();
  command.End(false);
  EXPECT_EQ(1, count);
}

TEST(DeferredCommandTest, DeferredRequirements) {
  TestSubsystem subsystem;
  DeferredCommand command{cmd::None, {&subsystem}};

  EXPECT_TRUE(command.GetRequirements().contains(&subsystem));
}
