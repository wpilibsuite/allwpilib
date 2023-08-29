// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "frc2/command/Commands.h"
#include "frc2/command/DeferredCommand.h"

using namespace frc2;

class DeferredCommandTest : public CommandTestBase {
 public:
  void DeferredFunctionsTest(bool interrupted) {
    std::unique_ptr<MockCommand> innerHolder = std::make_unique<MockCommand>();
    MockCommand* innerCommand = innerHolder.get();
    DeferredCommand deferred{[innerCommand] { return innerCommand; }, {}};

    EXPECT_CALL(*innerCommand, Initialize());
    EXPECT_CALL(*innerCommand, Execute());
    EXPECT_CALL(*innerCommand, End(interrupted));

    deferred.Initialize();
    deferred.Execute();
    EXPECT_FALSE(deferred.IsFinished());
    innerCommand->SetFinished(true);
    EXPECT_TRUE(deferred.IsFinished());
    deferred.End(interrupted);
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

TEST_F(DeferredCommandTest, DeferredNullCommand) {
  DeferredCommand command{[] { return nullptr; }, {}};

  EXPECT_NO_THROW({
    command.Initialize();
    command.Execute();
    command.IsFinished();
    command.End(false);
  });
}
