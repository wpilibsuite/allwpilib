// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class StartEndCommandTest extends CommandTestBase {
  @Test
  void startEndCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder cond1 = new ConditionHolder();
      ConditionHolder cond2 = new ConditionHolder();

      StartEndCommand command =
          new StartEndCommand(() -> cond1.setCondition(true), () -> cond2.setCondition(true));

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));

      scheduler.cancel(command);

      assertFalse(scheduler.isScheduled(command));
      assertTrue(cond1.getCondition());
      assertTrue(cond2.getCondition());
    }
  }
}
