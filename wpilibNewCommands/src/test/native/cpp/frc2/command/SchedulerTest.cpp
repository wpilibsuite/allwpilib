// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <frc/simulation/DriverStationSim.h>

#include "CommandTestBase.h"
#include "frc2/command/InstantCommand.h"
#include "frc2/command/RunCommand.h"
#include "frc2/command/Subsystem.h"

using namespace frc2;
class SchedulerTest : public CommandTestBase {};

TEST_F(SchedulerTest, SchedulerLambdaTestNoInterrupt) {
  CommandScheduler scheduler = GetScheduler();

  InstantCommand command;

  int counter = 0;

  scheduler.OnCommandInitialize([&counter](const Command&) { counter++; });
  scheduler.OnCommandExecute([&counter](const Command&) { counter++; });
  scheduler.OnCommandFinish([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();

  EXPECT_EQ(counter, 3);
}

TEST_F(SchedulerTest, SchedulerLambdaInterruptTest) {
  CommandScheduler scheduler = GetScheduler();

  RunCommand command([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Run();
  scheduler.Cancel(&command);

  EXPECT_EQ(counter, 1);
}

TEST_F(SchedulerTest, UnregisterSubsystemTest) {
  CommandScheduler scheduler = GetScheduler();

  TestSubsystem system;

  scheduler.RegisterSubsystem(&system);

  EXPECT_NO_FATAL_FAILURE(scheduler.UnregisterSubsystem(&system));
}

TEST_F(SchedulerTest, SchedulerCancelAllTest) {
  CommandScheduler scheduler = GetScheduler();

  RunCommand command([] {}, {});
  RunCommand command2([] {}, {});

  int counter = 0;

  scheduler.OnCommandInterrupt([&counter](const Command&) { counter++; });

  scheduler.Schedule(&command);
  scheduler.Schedule(&command2);
  scheduler.Run();
  scheduler.CancelAll();

  EXPECT_EQ(counter, 2);
}

class CounterSubsystem : public Subsystem {
 public:
  int counter{0};
  CounterSubsystem() {}
  void DisabledInit() override { counter++; }
};

TEST_F(SchedulerTest, DisabledInitTest) {
  using namespace frc::sim;
  CommandScheduler scheduler = GetScheduler();
  CounterSubsystem subsystem{};
  scheduler.RegisterSubsystem(&subsystem);

  DriverStationSim::SetEnabled(false);
  DriverStationSim::NotifyNewData();
  scheduler.Run();
  EXPECT_EQ(1, subsystem.counter) << "DisabledInit() didn't run";

  scheduler.Run();
  EXPECT_EQ(1, subsystem.counter) << "DisabledInit() ran twice on same disable";

  DriverStationSim::SetEnabled(true);
  DriverStationSim::NotifyNewData();
  scheduler.Run();
  EXPECT_EQ(1, subsystem.counter) << "DisabledInit() ran when enabled";

  DriverStationSim::SetEnabled(false);
  DriverStationSim::NotifyNewData();
  scheduler.Run();
  EXPECT_EQ(2, subsystem.counter) << "DisabledInit() didn't run on re-disable";
}
