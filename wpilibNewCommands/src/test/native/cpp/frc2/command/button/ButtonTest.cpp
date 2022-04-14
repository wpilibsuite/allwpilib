// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>

#include "../CommandTestBase.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/button/Trigger.h"
#include "gtest/gtest.h"

using namespace frc2;
class ButtonTest : public CommandTestBase {};

TEST_F(ButtonTest, WhenPressed) {
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

TEST_F(ButtonTest, WhenReleased) {
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

TEST_F(ButtonTest, WhileHeld) {
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

TEST_F(ButtonTest, WhenHeld) {
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

TEST_F(ButtonTest, ToggleWhenPressed) {
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

TEST_F(ButtonTest, And) {
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

TEST_F(ButtonTest, Or) {
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

TEST_F(ButtonTest, Negate) {
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

TEST_F(ButtonTest, RValueButton) {
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

TEST_F(ButtonTest, Debounce) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool pressed = false;
  RunCommand command([] {});

  Trigger([&pressed] { return pressed; }).Debounce(100_ms).WhenActive(&command);
  pressed = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  frc::sim::StepTiming(300_ms);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}
