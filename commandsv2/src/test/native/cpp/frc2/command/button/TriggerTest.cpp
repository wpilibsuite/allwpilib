// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <frc/simulation/SimHooks.h>
#include <gtest/gtest.h>

#include "../CommandTestBase.h"
#include "frc2/command/CommandPtr.h"
#include "frc2/command/CommandScheduler.h"
#include "frc2/command/Commands.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/WaitUntilCommand.h"
#include "frc2/command/button/Trigger.h"

using namespace frc2;
class TriggerTest : public CommandTestBase {};

TEST_F(TriggerTest, OnTrue) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;

  WaitUntilCommand command([&finished] { return finished; });

  Trigger([&pressed] { return pressed; }).OnTrue(&command);
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
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = false;
  WaitUntilCommand command([&finished] { return finished; });

  pressed = true;
  Trigger([&pressed] { return pressed; }).OnFalse(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
}

TEST_F(TriggerTest, OnChange) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = true;
  WaitUntilCommand command([&finished] { return finished; });

  Trigger([&pressed] { return pressed; }).OnChange(&command);
  scheduler.Run();
  EXPECT_FALSE(command.IsScheduled());
  pressed = false;
  scheduler.Run();
  EXPECT_TRUE(command.IsScheduled());
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(command.IsScheduled());
  finished = false;
  pressed = true;
  scheduler.Run();
  EXPECT_TRUE(command.IsScheduled());
  finished = true;
  scheduler.Run();
  EXPECT_FALSE(command.IsScheduled());
}

TEST_F(TriggerTest, WhileTrueRepeatedly) {
  auto& scheduler = CommandScheduler::GetInstance();
  int inits = 0;
  int counter = 0;
  bool pressed = false;
  CommandPtr command =
      FunctionalCommand([&inits] { inits++; }, [] {}, [](bool interrupted) {},
                        [&counter] { return ++counter % 2 == 0; })
          .Repeatedly();

  pressed = false;
  Trigger([&pressed] { return pressed; }).WhileTrue(std::move(command));
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
  auto& scheduler = CommandScheduler::GetInstance();
  int counter = 0;
  bool pressed = false;
  CommandPtr command = cmd::Run([&counter] { counter++; });

  pressed = false;
  Trigger([&pressed] { return pressed; }).WhileTrue(std::move(command));
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
  auto& scheduler = CommandScheduler::GetInstance();
  int startCounter = 0;
  int endCounter = 0;
  bool pressed = false;

  CommandPtr command = cmd::StartEnd([&startCounter] { startCounter++; },
                                     [&endCounter] { endCounter++; });

  pressed = false;
  Trigger([&pressed] { return pressed; }).WhileTrue(std::move(command));
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
  auto& scheduler = CommandScheduler::GetInstance();
  bool pressed = false;
  int startCounter = 0;
  int endCounter = 0;
  CommandPtr command = cmd::StartEnd([&startCounter] { startCounter++; },
                                     [&endCounter] { endCounter++; });

  Trigger([&pressed] { return pressed; }).ToggleOnTrue(std::move(command));
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
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed1 = false;
  bool pressed2 = false;
  WaitUntilCommand command([&finished] { return finished; });

  (Trigger([&pressed1] { return pressed1; }) && ([&pressed2] {
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
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed1 = false;
  bool pressed2 = false;
  WaitUntilCommand command1([&finished] { return finished; });
  WaitUntilCommand command2([&finished] { return finished; });

  (Trigger([&pressed1] { return pressed1; }) || ([&pressed2] {
     return pressed2;
   })).OnTrue(&command1);
  pressed1 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command1));

  pressed1 = false;

  (Trigger([&pressed1] { return pressed1; }) || ([&pressed2] {
     return pressed2;
   })).OnTrue(&command2);
  pressed2 = true;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command2));
}

TEST_F(TriggerTest, Negate) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool finished = false;
  bool pressed = true;
  WaitUntilCommand command([&finished] { return finished; });

  (!Trigger([&pressed] { return pressed; })).OnTrue(&command);
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));
  pressed = false;
  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}

TEST_F(TriggerTest, Debounce) {
  auto& scheduler = CommandScheduler::GetInstance();
  bool pressed = false;
  RunCommand command([] {});

  Trigger([&pressed] { return pressed; }).Debounce(100_ms).OnTrue(&command);
  pressed = true;
  scheduler.Run();
  EXPECT_FALSE(scheduler.IsScheduled(&command));

  frc::sim::StepTiming(300_ms);

  scheduler.Run();
  EXPECT_TRUE(scheduler.IsScheduled(&command));
}
