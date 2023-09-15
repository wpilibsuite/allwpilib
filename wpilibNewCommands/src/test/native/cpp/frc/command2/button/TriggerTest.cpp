// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/SimHooks.h>
#include <gtest/gtest.h>

#include "../CommandTestBase.h"
#include "frc/command2/CommandPtr.h"
#include "frc/command2/CommandScheduler.h"
#include "frc/command2/Commands.h"
#include "frc/command2/RunCommand.h"
#include "frc/command2/WaitUntilCommand.h"
#include "frc/command2/button/Trigger.h"

class TriggerTest : public CommandTestBase {};

TEST_F(TriggerTest, OnTrue) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;

  frc::WaitUntilCommand command([&finished] { return finished; });

  frc::Trigger([&pressed] { return pressed; }).OnTrue(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(TriggerTest, OnFalse) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;
  frc::WaitUntilCommand command([&finished] { return finished; });

  pressed = true;
  frc::Trigger([&pressed] { return pressed; }).OnFalse(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(TriggerTest, WhileTrueRepeatedly) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  int inits = 0;
  int counter = 0;
  bool pressed = false;
  frc::CommandPtr command =
      frc::FunctionalCommand([&inits] { inits++; }, [] {},
                             [](bool interrupted) {},
                             [&counter] { return ++counter % 2 == 0; })
          .Repeatedly();

  pressed = false;
  frc::Trigger([&pressed] { return pressed; }).WhileTrue(std::move(command));
  scheduler.Run();
  EXPECT_EQ(0, inits);
  pressed = true;
  scheduler.Run();
  EXPECT_EQ(1, inits);
  scheduler.Run();
  EXPECT_EQ(1, inits);
  scheduler.Run();
  EXPECT_EQ(2, inits);
  pressed = false;
  scheduler.Run();
  EXPECT_EQ(2, inits);
}

TEST_F(TriggerTest, WhileTrueLambdaRun) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  int counter = 0;
  bool pressed = false;
  frc::CommandPtr command = frc::cmd::Run([&counter] { counter++; });

  pressed = false;
  frc::Trigger([&pressed] { return pressed; }).WhileTrue(std::move(command));
  scheduler.Run();
  EXPECT_EQ(0, counter);
  pressed = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_EQ(2, counter);
  pressed = false;
  scheduler.Run();
  EXPECT_EQ(2, counter);
}

TEST_F(TriggerTest, WhenTrueOnce) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  int startCounter = 0;
  int endCounter = 0;
  bool pressed = false;

  frc::CommandPtr command = frc::cmd::StartEnd(
      [&startCounter] { startCounter++; }, [&endCounter] { endCounter++; });

  pressed = false;
  frc::Trigger([&pressed] { return pressed; }).WhileTrue(std::move(command));
  scheduler.Run();
  EXPECT_EQ(0, startCounter);
  EXPECT_EQ(0, endCounter);
  pressed = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_EQ(1, startCounter);
  EXPECT_EQ(0, endCounter);
  pressed = false;
  scheduler.Run();
  EXPECT_EQ(1, startCounter);
  EXPECT_EQ(1, endCounter);
}

TEST_F(TriggerTest, ToggleOnTrue) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  bool pressed = false;
  int startCounter = 0;
  int endCounter = 0;
  frc::CommandPtr command = frc::cmd::StartEnd(
      [&startCounter] { startCounter++; }, [&endCounter] { endCounter++; });

  frc::Trigger([&pressed] { return pressed; }).ToggleOnTrue(std::move(command));
  scheduler.Run();
  EXPECT_EQ(0, startCounter);
  EXPECT_EQ(0, endCounter);
  pressed = true;
  scheduler.Run();
  scheduler.Run();
  EXPECT_EQ(1, startCounter);
  EXPECT_EQ(0, endCounter);
  pressed = false;
  scheduler.Run();
  EXPECT_EQ(1, startCounter);
  EXPECT_EQ(0, endCounter);
  pressed = true;
  scheduler.Run();
  EXPECT_EQ(1, startCounter);
  EXPECT_EQ(1, endCounter);
}

TEST_F(TriggerTest, And) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed1 = false;
  bool pressed2 = false;
  frc::WaitUntilCommand command([&finished] { return finished; });

  (frc::Trigger([&pressed1] { return pressed1; }) && ([&pressed2] {
     return pressed2;
   })).OnTrue(&command);
  pressed1 = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed2 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}

TEST_F(TriggerTest, Or) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed1 = false;
  bool pressed2 = false;
  frc::WaitUntilCommand command1([&finished] { return finished; });
  frc::WaitUntilCommand command2([&finished] { return finished; });

  (frc::Trigger([&pressed1] { return pressed1; }) || ([&pressed2] {
     return pressed2;
   })).OnTrue(&command1);
  pressed1 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command1));

  pressed1 = false;

  (frc::Trigger([&pressed1] { return pressed1; }) || ([&pressed2] {
     return pressed2;
   })).OnTrue(&command2);
  pressed2 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command2));
}

TEST_F(TriggerTest, Negate) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = true;
  frc::WaitUntilCommand command([&finished] { return finished; });

  (!frc::Trigger([&pressed] { return pressed; })).OnTrue(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}

TEST_F(TriggerTest, Debounce) {
  auto& scheduler = frc::CommandScheduler::GetInstance();
  bool pressed = false;
  frc::RunCommand command([] {});

  frc::Trigger([&pressed] { return pressed; })
      .Debounce(100_ms)
      .OnTrue(&command);
  pressed = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  frc::sim::StepTiming(300_ms);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}
