/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "CommandTestBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/button/Trigger.h"
#include "gtest/gtest.h"

using namespace frc2;
class ButtonTest : public CommandTestBase {};

TEST_F(ButtonTest, WhenPressedTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;

  WaitUntilCommand command([&finished] { return finished; });

  Trigger([&pressed] { return pressed; }).WhenActive(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ButtonTest, WhenReleasedTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;
  WaitUntilCommand command([&finished] { return finished; });

  pressed = true;
  Trigger([&pressed] { return pressed; }).WhenInactive(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ButtonTest, WhileHeldTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;
  WaitUntilCommand command([&finished] { return finished; });

  pressed = false;
  Trigger([&pressed] { return pressed; }).WhileActiveContinous(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  finished = false;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ButtonTest, WhenHeldTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;
  WaitUntilCommand command([&finished] { return finished; });

  pressed = false;
  Trigger([&pressed] { return pressed; }).WhileActiveOnce(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  finished = false;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  pressed = false;
  Trigger([&pressed] { return pressed; }).WhileActiveOnce(&command);
  pressed = true;
  scheduler.Run();
  pressed = false;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ButtonTest, ToggleWhenPressedTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;
  WaitUntilCommand command([&finished] { return finished; });

  pressed = false;
  Trigger([&pressed] { return pressed; }).ToggleWhenActive(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  pressed = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(ButtonTest, AndTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed1 = false;
  bool pressed2 = false;
  WaitUntilCommand command([&finished] { return finished; });

  (Trigger([&pressed1] { return pressed1; }) && Trigger([&pressed2] {
     return pressed2;
   })).WhenActive(&command);
  pressed1 = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed2 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}

TEST_F(ButtonTest, OrTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed1 = false;
  bool pressed2 = false;
  WaitUntilCommand command1([&finished] { return finished; });
  WaitUntilCommand command2([&finished] { return finished; });

  (Trigger([&pressed1] { return pressed1; }) || Trigger([&pressed2] {
     return pressed2;
   })).WhenActive(&command1);
  pressed1 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command1));

  pressed1 = false;

  (Trigger([&pressed1] { return pressed1; }) || Trigger([&pressed2] {
     return pressed2;
   })).WhenActive(&command2);
  pressed2 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command2));
}

TEST_F(ButtonTest, NegateTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = true;
  WaitUntilCommand command([&finished] { return finished; });

  (!Trigger([&pressed] { return pressed; })).WhenActive(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}

TEST_F(ButtonTest, RValueButtonTest) {
  auto& scheduler = CommandScheduler::GetInstance();
  int counter = 0;
  bool pressed = false;

  RunCommand command([&counter] { counter++; }, {});

  Trigger([&pressed] { return pressed; }).WhenActive(std::move(command));
  scheduler.Run();
  EXPECT_EQ(counter, 0);
  pressed = true;
  scheduler.Run();
  EXPECT_EQ(counter, 1);
}
