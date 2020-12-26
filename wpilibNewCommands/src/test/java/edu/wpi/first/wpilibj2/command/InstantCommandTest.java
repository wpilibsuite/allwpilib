// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class InstantCommandTest extends CommandTestBase {
  @Test
  void instantCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder cond = new ConditionHolder();

      InstantCommand command = new InstantCommand(() -> cond.setCondition(true));

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(cond.getCondition());
      assertFalse(scheduler.isScheduled(command));
    }
  }
}
