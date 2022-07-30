// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class EndlessCommandTest extends CommandTestBase {
  @Test
  void endlessCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      EndlessCommand command = new EndlessCommand(new InstantCommand());

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));
    }
  }
}
