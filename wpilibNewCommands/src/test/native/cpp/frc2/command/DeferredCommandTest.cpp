// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/Commands.h"
#include "frc2/command/DeferredCommand.h"
#include "frc2/command/FunctionalCommand.h"

using namespace frc2;

class DeferredCommandTest : public CommandTestBase {
 public:
  void DeferredFunctionsTest(bool testInterrupted) {
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
                                     EXPECT_EQ(interrupted, testInterrupted);
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
    deferred.End(testInterrupted);
    EXPECT_EQ(1, endCount);
  }
};

TEST_F(DeferredCommandTest, DeferredFunctions) {
  DeferredFunctionsTest(false);
  DeferredFunctionsTest(true);
}

TEST_F(DeferredCommandTest, DeferredSupplierOnlyCalledDuringInit) {
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

TEST_F(DeferredCommandTest, DeferredRequirements) {
  TestSubsystem subsystem;
  DeferredCommand command{cmd::None, {&subsystem}};

  EXPECT_TRUE(command.GetRequirements().contains(&subsystem));
}
