/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class WaitUntilCommandTest extends CommandTestBase {
  @Test
  void waitUntilTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      ConditionHolder condition = new ConditionHolder();

      Command command = new WaitUntilCommand(condition::getCondition);

      scheduler.schedule(command);
      scheduler.run();
      assertTrue(scheduler.isScheduled(command));
      condition.setCondition(true);
      scheduler.run();
      assertFalse(scheduler.isScheduled(command));
    }
  }
}
