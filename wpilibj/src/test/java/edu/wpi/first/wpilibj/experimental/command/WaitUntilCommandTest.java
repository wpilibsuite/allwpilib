package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class WaitUntilCommandTest extends CommandTestBase {
  @Test
  void waitUntilTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ConditionHolder condition = new ConditionHolder();

    Command command = new WaitUntilCommand(condition::getCondition);

    scheduler.scheduleCommand(command, true);
    scheduler.run();
    assertTrue(scheduler.isScheduled(command));
    condition.setCondition(true);
    scheduler.run();
    assertFalse(scheduler.isScheduled(command));
  }
}
