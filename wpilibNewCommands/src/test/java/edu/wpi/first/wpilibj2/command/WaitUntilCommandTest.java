// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
