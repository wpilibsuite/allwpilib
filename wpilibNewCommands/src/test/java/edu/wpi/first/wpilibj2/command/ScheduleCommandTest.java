// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

class ScheduleCommandTest extends CommandTestBase {
  @Test
  void scheduleCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      ScheduleCommand scheduleCommand = new ScheduleCommand(command1, command2);

      scheduler.schedule(scheduleCommand);

      verify(command1).schedule();
      verify(command2).schedule();
    }
  }

  @Test
  void scheduleCommandDuringRunTest() {
    try (CommandScheduler scheduler = CommandScheduler.getInstance()) {
      InstantCommand toSchedule = new InstantCommand();
      ScheduleCommand scheduleCommand = new ScheduleCommand(toSchedule);
      SequentialCommandGroup group =
          new SequentialCommandGroup(new InstantCommand(), scheduleCommand);

      scheduler.schedule(group);
      scheduler.schedule(new RunCommand(() -> {}));
      scheduler.run();
      assertDoesNotThrow(scheduler::run);
    }
  }
}
