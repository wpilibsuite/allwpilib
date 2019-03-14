package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;

public class DefaultICommandTest extends ICommandTestBase {
  @Test
  void defaultCommandScheduleTest() {
    SchedulerNew scheduler = new SchedulerNew();

    ASubsystem hasDefaultCommand = new ASubsystem();

    MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
    ICommand defaultCommand = defaultHolder.getMock();
    hasDefaultCommand.init(defaultCommand);

    scheduler.registerSubsystem(hasDefaultCommand);
    scheduler.run();

    assertTrue(scheduler.isScheduled(defaultCommand));
  }

  @Test
  void defaultCommandInterruptResumeTest() {
    SchedulerNew scheduler = new SchedulerNew();

    ASubsystem hasDefaultCommand = new ASubsystem();

    MockCommandHolder defaultHolder = new MockCommandHolder(true, hasDefaultCommand);
    ICommand defaultCommand = defaultHolder.getMock();
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, hasDefaultCommand);
    ICommand interrupter = interrupterHolder.getMock();
    hasDefaultCommand.init(defaultCommand);

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
