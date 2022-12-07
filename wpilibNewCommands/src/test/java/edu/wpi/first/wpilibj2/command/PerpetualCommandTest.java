// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class PerpetualCommandTest extends CommandTestBase {
  @SuppressWarnings("removal") // PerpetualCommand
  @Test
  void perpetualCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      PerpetualCommand command = new PerpetualCommand(new InstantCommand());

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));
    }
  }
}
