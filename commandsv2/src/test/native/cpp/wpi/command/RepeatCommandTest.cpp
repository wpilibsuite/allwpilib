// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/RepeatCommand.hpp"

#include "CommandTestBase.hpp"
#include "CompositionTestBase.hpp"
#include "wpi/commands2/FunctionalCommand.hpp"

using namespace wpi::cmd;
class RepeatCommandTest : public CommandTestBase {};

TEST_CASE_METHOD(RepeatCommandTest, "RepeatCommandTest CallsMethodsCorrectly",
                 "[commandsv2][command]") {
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

  CHECK(0 == initCounter);
  CHECK(0 == exeCounter);
  CHECK(0 == isFinishedCounter);
  CHECK(0 == endCounter);

  scheduler.Schedule(command);
  CHECK(1 == initCounter);
  CHECK(0 == exeCounter);
  CHECK(0 == isFinishedCounter);
  CHECK(0 == endCounter);

  isFinishedHook = false;
  scheduler.Run();
  CHECK(1 == initCounter);
  CHECK(1 == exeCounter);
  CHECK(1 == isFinishedCounter);
  CHECK(0 == endCounter);

  isFinishedHook = true;
  scheduler.Run();
  CHECK(1 == initCounter);
  CHECK(2 == exeCounter);
  CHECK(2 == isFinishedCounter);
  CHECK(1 == endCounter);

  isFinishedHook = false;
  scheduler.Run();
  CHECK(2 == initCounter);
  CHECK(3 == exeCounter);
  CHECK(3 == isFinishedCounter);
  CHECK(1 == endCounter);

  isFinishedHook = true;
  scheduler.Run();
  CHECK(2 == initCounter);
  CHECK(4 == exeCounter);
  CHECK(4 == isFinishedCounter);
  CHECK(2 == endCounter);

  command.Cancel();
  CHECK(2 == initCounter);
  CHECK(4 == exeCounter);
  CHECK(4 == isFinishedCounter);
  CHECK(2 == endCounter);
}

INSTANTIATE_SINGLE_COMMAND_COMPOSITION_TEST_SUITE(RepeatCommandTest,
                                                  RepeatCommand);
