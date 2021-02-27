// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "command/MockCommand.h"
#include "frc/DriverStation.h"
#include "frc/RobotState.h"
#include "frc/Timer.h"
#include "frc/commands/CommandGroup.h"
#include "frc/commands/Scheduler.h"
#include "frc/commands/Subsystem.h"
#include "gtest/gtest.h"

using namespace frc;

class CommandTest : public testing::Test {
 protected:
  void SetUp() override { Scheduler::GetInstance()->SetEnabled(true); }

  /**
   * Tears Down the Scheduler at the end of each test.
   * Must be called at the end of each test inside each test in order to prevent
   * them being deallocated
   * when they leave the scope of the test (causing a segfault).
   * This can not be done within the virtual void Teardown() method because it
   * is called outside of the
   * scope of the test.
   */
  void TeardownScheduler() { Scheduler::GetInstance()->ResetAll(); }

  void AssertCommandState(MockCommand* command, int32_t initialize,
                          int32_t execute, int32_t isFinished, int32_t end,
                          int32_t interrupted) {
    EXPECT_EQ(initialize, command->GetInitializeCount());
    EXPECT_EQ(execute, command->GetExecuteCount());
    EXPECT_EQ(isFinished, command->GetIsFinishedCount());
    EXPECT_EQ(end, command->GetEndCount());
    EXPECT_EQ(interrupted, command->GetInterruptedCount());
  }
};

class ASubsystem : public Subsystem {
 private:
  Command* m_command = nullptr;

 public:
  explicit ASubsystem(const std::string& name) : Subsystem(name) {}

  void InitDefaultCommand() override {
    if (m_command != nullptr) {
      SetDefaultCommand(m_command);
    }
  }

  void Init(Command* command) { m_command = command; }
};

// CommandParallelGroupTest ported from CommandParallelGroupTest.java
TEST_F(CommandTest, ParallelCommands) {
  auto command1 = new MockCommand;
  auto command2 = new MockCommand;
  CommandGroup commandGroup;

  commandGroup.AddParallel(command1);
  commandGroup.AddParallel(command2);

  AssertCommandState(command1, 0, 0, 0, 0, 0);
  AssertCommandState(command2, 0, 0, 0, 0, 0);
  commandGroup.Start();
  AssertCommandState(command1, 0, 0, 0, 0, 0);
  AssertCommandState(command2, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 0, 0, 0, 0, 0);
  AssertCommandState(command2, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 0);
  AssertCommandState(command2, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 2, 2, 0, 0);
  AssertCommandState(command2, 1, 2, 2, 0, 0);
  command1->SetHasFinished(true);
  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 3, 3, 1, 0);
  AssertCommandState(command2, 1, 3, 3, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 3, 3, 1, 0);
  AssertCommandState(command2, 1, 4, 4, 0, 0);
  command2->SetHasFinished(true);
  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 3, 3, 1, 0);
  AssertCommandState(command2, 1, 5, 5, 1, 0);

  TeardownScheduler();
}
// END CommandParallelGroupTest

// CommandScheduleTest ported from CommandScheduleTest.java
TEST_F(CommandTest, RunAndTerminate) {
  MockCommand command;
  command.Start();
  AssertCommandState(&command, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 2, 2, 0, 0);
  command.SetHasFinished(true);
  AssertCommandState(&command, 1, 2, 2, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 3, 3, 1, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 3, 3, 1, 0);

  TeardownScheduler();
}

TEST_F(CommandTest, RunAndCancel) {
  MockCommand command;
  command.Start();
  AssertCommandState(&command, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 2, 2, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 3, 3, 0, 0);
  command.Cancel();
  AssertCommandState(&command, 1, 3, 3, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 3, 3, 0, 1);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 3, 3, 0, 1);

  TeardownScheduler();
}
// END CommandScheduleTest

// CommandSequentialGroupTest ported from CommandSequentialGroupTest.java
TEST_F(CommandTest, ThreeCommandOnSubSystem) {
  ASubsystem subsystem("Three Command Test Subsystem");
  auto command1 = new MockCommand;
  command1->Requires(&subsystem);
  auto command2 = new MockCommand;
  command2->Requires(&subsystem);
  auto command3 = new MockCommand;
  command3->Requires(&subsystem);

  CommandGroup commandGroup;
  commandGroup.AddSequential(command1, 1.0);
  commandGroup.AddSequential(command2, 2.0);
  commandGroup.AddSequential(command3);

  AssertCommandState(command1, 0, 0, 0, 0, 0);
  AssertCommandState(command2, 0, 0, 0, 0, 0);
  AssertCommandState(command3, 0, 0, 0, 0, 0);

  commandGroup.Start();
  AssertCommandState(command1, 0, 0, 0, 0, 0);
  AssertCommandState(command2, 0, 0, 0, 0, 0);
  AssertCommandState(command3, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 0, 0, 0, 0, 0);
  AssertCommandState(command2, 0, 0, 0, 0, 0);
  AssertCommandState(command3, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 0);
  AssertCommandState(command2, 0, 0, 0, 0, 0);
  AssertCommandState(command3, 0, 0, 0, 0, 0);
  Wait(1);  // command 1 timeout

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 1);
  AssertCommandState(command2, 1, 1, 1, 0, 0);
  AssertCommandState(command3, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 1);
  AssertCommandState(command2, 1, 2, 2, 0, 0);
  AssertCommandState(command3, 0, 0, 0, 0, 0);
  Wait(2);  // command 2 timeout

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 1);
  AssertCommandState(command2, 1, 2, 2, 0, 1);
  AssertCommandState(command3, 1, 1, 1, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 1);
  AssertCommandState(command2, 1, 2, 2, 0, 1);
  AssertCommandState(command3, 1, 2, 2, 0, 0);
  command3->SetHasFinished(true);
  AssertCommandState(command1, 1, 1, 1, 0, 1);
  AssertCommandState(command2, 1, 2, 2, 0, 1);
  AssertCommandState(command3, 1, 2, 2, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 1);
  AssertCommandState(command2, 1, 2, 2, 0, 1);
  AssertCommandState(command3, 1, 3, 3, 1, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(command1, 1, 1, 1, 0, 1);
  AssertCommandState(command2, 1, 2, 2, 0, 1);
  AssertCommandState(command3, 1, 3, 3, 1, 0);

  TeardownScheduler();
}
// END CommandSequentialGroupTest

// CommandSequentialGroupTest ported from CommandSequentialGroupTest.java
TEST_F(CommandTest, OneCommandSupersedingAnotherBecauseOfDependencies) {
  ASubsystem subsystem("Command Superseding Test Subsystem");
  MockCommand command1;
  command1.Requires(&subsystem);
  MockCommand command2;
  command2.Requires(&subsystem);

  AssertCommandState(&command1, 0, 0, 0, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  command1.Start();
  AssertCommandState(&command1, 0, 0, 0, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 0, 0, 0, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 1, 1, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 2, 2, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 3, 3, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  command2.Start();
  AssertCommandState(&command1, 1, 3, 3, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 4, 4, 0, 1);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 4, 4, 0, 1);
  AssertCommandState(&command2, 1, 1, 1, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 4, 4, 0, 1);
  AssertCommandState(&command2, 1, 2, 2, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 4, 4, 0, 1);
  AssertCommandState(&command2, 1, 3, 3, 0, 0);

  TeardownScheduler();
}

TEST_F(CommandTest,
       OneCommandFailingSupersedingBecauseFirstCanNotBeInterrupted) {
  ASubsystem subsystem("Command Superseding Test Subsystem");
  MockCommand command1;

  command1.Requires(&subsystem);

  command1.SetInterruptible(false);
  MockCommand command2;
  command2.Requires(&subsystem);

  AssertCommandState(&command1, 0, 0, 0, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  command1.Start();
  AssertCommandState(&command1, 0, 0, 0, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 0, 0, 0, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 1, 1, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 2, 2, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 3, 3, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  command2.Start();
  AssertCommandState(&command1, 1, 3, 3, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  Scheduler::GetInstance()->Run();
  AssertCommandState(&command1, 1, 4, 4, 0, 0);
  AssertCommandState(&command2, 0, 0, 0, 0, 0);

  TeardownScheduler();
}

// END CommandSequentialGroupTest

class ModifiedMockCommand : public MockCommand {
 public:
  ModifiedMockCommand() : MockCommand() { SetTimeout(2.0); }
  bool IsFinished() override {
    return MockCommand::IsFinished() || IsTimedOut();
  }
};

TEST_F(CommandTest, TwoSecondTimeout) {
  ASubsystem subsystem("Two Second Timeout Test Subsystem");
  ModifiedMockCommand command;
  command.Requires(&subsystem);

  command.Start();
  AssertCommandState(&command, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 2, 2, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 3, 3, 0, 0);
  Wait(2);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 4, 4, 1, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(&command, 1, 4, 4, 1, 0);

  TeardownScheduler();
}

TEST_F(CommandTest, DefaultCommandWhereTheInteruptingCommandEndsItself) {
  ASubsystem subsystem("Default Command Test Subsystem");
  auto defaultCommand = new MockCommand;
  defaultCommand->Requires(&subsystem);
  MockCommand anotherCommand;
  anotherCommand.Requires(&subsystem);

  AssertCommandState(defaultCommand, 0, 0, 0, 0, 0);
  subsystem.Init(defaultCommand);

  AssertCommandState(defaultCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 2, 2, 0, 0);

  anotherCommand.Start();
  AssertCommandState(defaultCommand, 1, 2, 2, 0, 0);
  AssertCommandState(&anotherCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 2, 2, 0, 0);
  anotherCommand.SetHasFinished(true);
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 2, 2, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 3, 3, 1, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 2, 4, 4, 0, 1);
  AssertCommandState(&anotherCommand, 1, 3, 3, 1, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 2, 5, 5, 0, 1);
  AssertCommandState(&anotherCommand, 1, 3, 3, 1, 0);

  TeardownScheduler();
}

TEST_F(CommandTest, DefaultCommandsInterruptingCommandCanceled) {
  ASubsystem subsystem("Default Command Test Subsystem");
  auto defaultCommand = new MockCommand;
  defaultCommand->Requires(&subsystem);
  MockCommand anotherCommand;
  anotherCommand.Requires(&subsystem);

  AssertCommandState(defaultCommand, 0, 0, 0, 0, 0);
  subsystem.Init(defaultCommand);
  AssertCommandState(defaultCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 2, 2, 0, 0);

  anotherCommand.Start();
  AssertCommandState(defaultCommand, 1, 2, 2, 0, 0);
  AssertCommandState(&anotherCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 2, 2, 0, 0);
  anotherCommand.Cancel();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 2, 2, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 1, 3, 3, 0, 1);
  AssertCommandState(&anotherCommand, 1, 2, 2, 0, 1);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 2, 4, 4, 0, 1);
  AssertCommandState(&anotherCommand, 1, 2, 2, 0, 1);
  Scheduler::GetInstance()->Run();
  AssertCommandState(defaultCommand, 2, 5, 5, 0, 1);
  AssertCommandState(&anotherCommand, 1, 2, 2, 0, 1);

  TeardownScheduler();
}
