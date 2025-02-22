// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <tuple>
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

// Initial state, pressed, rising, falling
using InitialStateTestData =
    std::tuple<Trigger::InitialState, bool, bool, bool>;

class TriggerInitialStateTest
    : public TriggerTest,
      public testing::WithParamInterface<InitialStateTestData> {};

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

TEST_P(TriggerInitialStateTest, OnTrue) {
  auto [initialState, pressed, rising, falling] = GetParam();
  auto& scheduler = CommandScheduler::GetInstance();
  RunCommand command([] {});
  bool shouldBeScheduled = rising;

  Trigger([pressed = pressed] {
    return pressed;
  }).OnTrue(&command, initialState);

  EXPECT_FALSE(scheduler.IsScheduled(&command));

  scheduler.Run();
  EXPECT_EQ(shouldBeScheduled, scheduler.IsScheduled(&command));
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

TEST_P(TriggerInitialStateTest, OnFalse) {
  auto [initialState, pressed, rising, falling] = GetParam();
  auto& scheduler = CommandScheduler::GetInstance();
  RunCommand command([] {});
  bool shouldBeScheduled = falling;

  Trigger([pressed = pressed] {
    return pressed;
  }).OnFalse(&command, initialState);

  EXPECT_FALSE(scheduler.IsScheduled(&command));

  scheduler.Run();
  EXPECT_EQ(shouldBeScheduled, scheduler.IsScheduled(&command));
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

TEST_P(TriggerInitialStateTest, OnChange) {
  auto [initialState, pressed, rising, falling] = GetParam();
  auto& scheduler = CommandScheduler::GetInstance();
  RunCommand command([] {});
  bool shouldBeScheduled = rising || falling;

  Trigger([pressed = pressed] {
    return pressed;
  }).OnChange(&command, initialState);

  EXPECT_FALSE(scheduler.IsScheduled(&command));

  scheduler.Run();
  EXPECT_EQ(shouldBeScheduled, scheduler.IsScheduled(&command));
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

TEST_P(TriggerInitialStateTest, ToggleOnTrue) {
  auto [initialState, pressed, rising, falling] = GetParam();
  auto& scheduler = CommandScheduler::GetInstance();
  RunCommand command([] {});
  bool shouldBeScheduled = rising;

  Trigger([pressed = pressed] {
    return pressed;
  }).ToggleOnTrue(&command, initialState);

  EXPECT_FALSE(scheduler.IsScheduled(&command));

  scheduler.Run();
  EXPECT_EQ(shouldBeScheduled, scheduler.IsScheduled(&command));
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

INSTANTIATE_TEST_SUITE_P(
    InitialStateTests, TriggerInitialStateTest,
    testing::Values(
        // Initial state, pressed, rising, falling
        std::tuple{Trigger::InitialState::kFalse, false, false, false},
        std::tuple{Trigger::InitialState::kFalse, true, true, false},
        std::tuple{Trigger::InitialState::kTrue, false, false, true},
        std::tuple{Trigger::InitialState::kTrue, true, false, false},
        std::tuple{Trigger::InitialState::kCondition, false, false, false},
        std::tuple{Trigger::InitialState::kCondition, true, false, false},
        std::tuple{Trigger::InitialState::kNegCondition, false, false, true},
        std::tuple{Trigger::InitialState::kNegCondition, true, true, false}));
