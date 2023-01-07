// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "CommandTestBase.h"
#include "CompositionTestBase.h"
#include "frc2/command/FunctionalCommand.h"
#include "frc2/command/RepeatCommand.h"

using namespace frc2;
class RepeatCommandTest : public CommandTestBase {};

TEST_F(RepeatCommandTest, CallsMethodsCorrectly) {
  CommandScheduler scheduler = GetScheduler();

  int initCounter = 0;
  int exeCounter = 0;
  int isFinishedCounter = 0;
  int endCounter = 0;
  bool isFinishedHook = false;

  auto command =
      FunctionalCommand([&initCounter] { initCounter++; },
                        [&exeCounter] { exeCounter++; },
                        [&endCounter](bool interrupted) { endCounter++; },
                        [&isFinishedCounter, &isFinishedHook] {
                          isFinishedCounter++;
                          return isFinishedHook;
                        })
          .Repeatedly();

  EXPECT_EQ(0, initCounter);
  EXPECT_EQ(0, exeCounter);
  EXPECT_EQ(0, isFinishedCounter);
  EXPECT_EQ(0, endCounter);

  scheduler.Schedule(command);
  EXPECT_EQ(1, initCounter);
  EXPECT_EQ(0, exeCounter);
  EXPECT_EQ(0, isFinishedCounter);
  EXPECT_EQ(0, endCounter);

  isFinishedHook = false;
  scheduler.Run();
  EXPECT_EQ(1, initCounter);
  EXPECT_EQ(1, exeCounter);
  EXPECT_EQ(1, isFinishedCounter);
  EXPECT_EQ(0, endCounter);

  isFinishedHook = true;
  scheduler.Run();
  EXPECT_EQ(1, initCounter);
  EXPECT_EQ(2, exeCounter);
  EXPECT_EQ(2, isFinishedCounter);
  EXPECT_EQ(1, endCounter);

  isFinishedHook = false;
  scheduler.Run();
  EXPECT_EQ(2, initCounter);
  EXPECT_EQ(3, exeCounter);
  EXPECT_EQ(3, isFinishedCounter);
  EXPECT_EQ(1, endCounter);
}

INSTANTIATE_SINGLE_COMMAND_COMPOSITION_TEST_SUITE(RepeatCommandTest,
                                                  RepeatCommand);
