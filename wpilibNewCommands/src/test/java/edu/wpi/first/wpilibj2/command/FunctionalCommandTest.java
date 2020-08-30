/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
