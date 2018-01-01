/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <DriverStation.h>
#include <RobotState.h>

#include "Commands/ConditionalCommand.h"
#include "Commands/Scheduler.h"
#include "command/MockCommand.h"
#include "command/MockConditionalCommand.h"
#include "gtest/gtest.h"

using namespace frc;

class ConditionalCommandTest : public testing::Test {
 public:
  MockConditionalCommand* m_command;
  MockCommand* m_onTrue;
  MockCommand* m_onFalse;

 protected:
  void SetUp() override {
    RobotState::SetImplementation(DriverStation::GetInstance());
    Scheduler::GetInstance()->SetEnabled(true);

    m_onTrue = new MockCommand();
    m_onFalse = new MockCommand();
    m_command = new MockConditionalCommand(m_onTrue, m_onFalse);
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

TEST_F(ConditionalCommandTest, DISABLED_OnFalseTest) {
  m_command->SetCondition(false);

  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();  // init command and select m_onFalse
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();  // init m_onFalse
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onFalse, 1, 1, 2, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onFalse, 1, 2, 4, 0, 0);

  EXPECT_TRUE(m_onFalse->GetInitializeCount() > 0)
      << "Did not initialize the false command";
  EXPECT_TRUE(m_onTrue->GetInitializeCount() == 0)
      << "Initialized the true command";

  TeardownScheduler();
}

TEST_F(ConditionalCommandTest, DISABLED_OnTrueCancelTest) {
  m_command->SetCondition(true);

  Scheduler::GetInstance()->AddCommand(m_command);
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 0, 0, 0, 0, 0);
  Scheduler::GetInstance()->Run();  // init command and select m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 1, 1, 1, 0, 0);
  Scheduler::GetInstance()->Run();  // init m_onTrue
  AssertCommandState(*m_onTrue, 0, 0, 0, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 2, 2, 2, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 1, 2, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 3, 3, 3, 0, 0);
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 2, 4, 0, 0);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 4, 4, 4, 0, 0);
  m_onTrue->Cancel();
  Scheduler::GetInstance()->Run();
  AssertCommandState(*m_onTrue, 1, 3, 6, 0, 1);
  AssertCommandState(*m_onFalse, 0, 0, 0, 0, 0);
  AssertConditionalCommandState(*m_command, 5, 5, 5, 1, 0);

  EXPECT_TRUE(m_onTrue->GetInitializeCount() > 0)
      << "Did not initialize the true command\n";
  EXPECT_TRUE(m_onFalse->GetInitializeCount() == 0)
      << "Initialized the false command\n";

  TeardownScheduler();
}
