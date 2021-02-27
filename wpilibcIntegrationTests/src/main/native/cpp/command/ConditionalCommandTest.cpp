// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "command/MockCommand.h"
#include "command/MockConditionalCommand.h"
#include "frc/DriverStation.h"
#include "frc/RobotState.h"
#include "frc/commands/ConditionalCommand.h"
#include "frc/commands/Scheduler.h"
#include "frc/commands/Subsystem.h"
#include "gtest/gtest.h"

using namespace frc;

class ConditionalCommandTest : public testing::Test {
 public:
  MockConditionalCommand* m_command;
  MockCommand* m_onTrue;
  MockCommand* m_onFalse;
  MockConditionalCommand* m_commandNull;
  Subsystem* m_subsystem;

 protected:
  void SetUp() override {
    Scheduler::GetInstance()->SetEnabled(true);

    m_subsystem = new Subsystem("MockSubsystem");
    m_onTrue = new MockCommand(m_subsystem);
    m_onFalse = new MockCommand(m_subsystem);
    m_command = new MockConditionalCommand(m_onTrue, m_onFalse);
    m_commandNull = new MockConditionalCommand(m_onTrue, nullptr);
  }

  void TearDown() override { delete m_command; }

  /**
   * Tears Down the Scheduler at the end of each test.
   *
   * Must be called at the end of each test inside each test in order to prevent
   * them being deallocated when they leave the scope of the test (causing a
   * segfault). This cannot be done within the virtual void Teardown() method
   * because it is called outside of the scope of the test.
   */
  void TeardownScheduler() { Scheduler::GetInstance()->ResetAll(); }

  void AssertCommandState(MockCommand& command, int32_t initialize,
                          int32_t execute, int32_t isFinished, int32_t end,
                          int32_t interrupted) {
    EXPECT_EQ(initialize, command.GetInitializeCount());
    EXPECT_EQ(execute, command.GetExecuteCount());
    EXPECT_EQ(isFinished, command.GetIsFinishedCount());
    EXPECT_EQ(end, command.GetEndCount());
    EXPECT_EQ(interrupted, command.GetInterruptedCount());
  }

  void AssertConditionalCommandState(MockConditionalCommand& command,
                                     int32_t initialize, int32_t execute,
                                     int32_t isFinished, int32_t end,
                                     int32_t interrupted) {
    EXPECT_EQ(initialize, command.GetInitializeCount());
    EXPECT_EQ(execute, command.GetExecuteCount());
    EXPECT_EQ(isFinished, command.GetIsFinishedCount());
    EXPECT_EQ(end, command.GetEndCount());
    EXPECT_EQ(interrupted, command.GetInterruptedCount());
  }
};

TEST_F(ConditionalCommandTest, OnTrueTest) {
  m_command->SetCondition(true);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 0, 0);
  SCOPED_TRACE("5");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 0);
  SCOPED_TRACE("6");
  m_onTrue->SetHasFinished(true);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);
  SCOPED_TRACE("7");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);

  EXPECT_TRUE(m_onTrue->GetInitializeCount() > 0)
      << "Did not initialize the true command\n";
  EXPECT_TRUE(m_onFalse->GetInitializeCount() == 0)
      << "Initialized the false command\n";

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, OnFalseTest) {
  m_command->SetCondition(false);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onFalse, 1, 1, 1, 0, 0);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 0, 0);
  SCOPED_TRACE("5");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onFalse, 1, 2, 2, 0, 0);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 0);
  SCOPED_TRACE("6");
  m_onFalse->SetHasFinished(true);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onFalse, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);
  SCOPED_TRACE("7");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onFalse, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);

  EXPECT_TRUE(m_onFalse->GetInitializeCount() > 0)
      << "Did not initialize the false command";
  EXPECT_TRUE(m_onTrue->GetInitializeCount() == 0)
      << "Initialized the true command";

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, CancelSubCommandTest) {
  m_command->SetCondition(true);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 0, 0);
  SCOPED_TRACE("5");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 0);
  SCOPED_TRACE("6");
  m_onTrue->Cancel();
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 1);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);
  SCOPED_TRACE("7");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 1);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, CancelCondCommandTest) {
  m_command->SetCondition(true);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 0, 0);
  SCOPED_TRACE("5");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 0);
  SCOPED_TRACE("6");
  m_command->Cancel();
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 1);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 1);
  SCOPED_TRACE("7");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 1);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 1);

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, OnTrueTwiceTest) {
  m_command->SetCondition(true);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 0, 0);
  SCOPED_TRACE("5");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 0);
  SCOPED_TRACE("6");
  m_onTrue->SetHasFinished(true);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);
  SCOPED_TRACE("7");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);

  m_onTrue->ResetCounters();
  m_command->ResetCounters();
  Scheduler::GetInstance()->AddCommand(m_command);

  SCOPED_TRACE("11");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("12");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("13");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("14");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 0, 0);
  SCOPED_TRACE("15");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 0);
  SCOPED_TRACE("16");
  m_onTrue->SetHasFinished(true);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);
  SCOPED_TRACE("17");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 1, 0);

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, OnTrueInstantTest) {
  m_command->SetCondition(true);
  m_onTrue->SetHasFinished(true);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 1, 0);
  SCOPED_TRACE("5");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 1, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 1, 0);

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, CancelRequiresTest) {
  m_command->SetCondition(true);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 1, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 2, 2, 0, 0);
  SCOPED_TRACE("5");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 2, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 3, 3, 0, 0);
  SCOPED_TRACE("6");
  m_onFalse->Start();
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 0, 1);
  SCOPED_TRACE("7");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 3, 0, 1);
  AssertCommandState(*m_onFalse, 1, 1, 1, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 4, 4, 0, 1);

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, OnFalseNullTest) {
  m_command->SetCondition(false);

  SCOPED_TRACE("1");
  Scheduler::GetInstance()->AddCommand(m_commandNull);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_commandNull, 0, 0, 0, 0, 0);
  SCOPED_TRACE("2");
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_commandNull, 0, 0, 0, 0, 0);
  SCOPED_TRACE("3");
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_commandNull, 1, 1, 1, 1, 0);
  SCOPED_TRACE("4");
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_commandNull, 1, 1, 1, 1, 0);

  TeardownScheduler();
}
