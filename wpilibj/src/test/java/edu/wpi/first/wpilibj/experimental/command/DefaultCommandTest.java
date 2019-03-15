package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class DefaultCommandTest extends CommandTestBase {
  @Test
  void defaultCommandScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ASubsystem hasDefaultCommand = new ASubsystem();

    MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
    Command defaultCommand = defaultHolder.getMock();
    hasDefaultCommand.init(defaultCommand);

    scheduler.registerSubsystem(hasDefaultCommand);
    scheduler.run();

    assertTrue(scheduler.isScheduled(defaultCommand));
  }

  @Test
  void defaultCommandInterruptResumeTest() {
    CommandScheduler scheduler = new CommandScheduler();

    ASubsystem hasDefaultCommand = new ASubsystem();

    MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
    Command defaultCommand = defaultHolder.getMock();
    hasDefaultCommand.init(defaultCommand);
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, hasDefaultCommand);
    Command interrupter = interrupterHolder.getMock();

    scheduler.registerSubsystem(hasDefaultCommand);
    scheduler.run();
    scheduler.scheduleCommand(interrupter, true);

    assertFalse(scheduler.isScheduled(defaultCommand));
    assertTrue(scheduler.isScheduled(interrupter));

    scheduler.cancelCommand(interrupter);
    scheduler.run();

    assertTrue(scheduler.isScheduled(defaultCommand));
    assertFalse(scheduler.isScheduled(interrupter));
  }
}
