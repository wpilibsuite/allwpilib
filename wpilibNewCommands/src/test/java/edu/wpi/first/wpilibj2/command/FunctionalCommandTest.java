// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class FunctionalCommandTest extends CommandTestBase {
  @Test
  void functionalCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder cond1 = new ConditionHolder();
      ConditionHolder cond2 = new ConditionHolder();
      ConditionHolder cond3 = new ConditionHolder();
      ConditionHolder cond4 = new ConditionHolder();

      FunctionalCommand command =
          new FunctionalCommand(() -> cond1.setCondition(true), () -> cond2.setCondition(true),
              interrupted -> cond3.setCondition(true), cond4::getCondition);

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));

      cond4.setCondition(true);

      scheduler.run();

      assertFalse(scheduler.isScheduled(command));
      assertTrue(cond1.getCondition());
      assertTrue(cond2.getCondition());
      assertTrue(cond3.getCondition());
    }
  }
}
