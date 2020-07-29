/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertTrue;

class PerpetualCommandTest extends CommandTestBase {
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
