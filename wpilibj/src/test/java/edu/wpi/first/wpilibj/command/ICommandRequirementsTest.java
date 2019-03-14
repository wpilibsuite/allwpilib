package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

public class ICommandRequirementsTest extends ICommandTestBase{
  @Test
  void requirementInterruptTest() {
    SchedulerNew scheduler = new SchedulerNew();

    Subsystem requirement = new ASubsystem();

    MockCommandHolder interruptedHolder = new MockCommandHolder(true, requirement);
    ICommand interrupted = interruptedHolder.getMock();
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
    ICommand interrupter = interrupterHolder.getMock();

    scheduler.scheduleCommand(interrupted, true);
    scheduler.run();
    scheduler.scheduleCommand(interrupter, true);
    scheduler.run();

    verify(interrupted).initialize();
    verify(interrupted).execute();
    verify(interrupted).interrupted();
    verify(interrupted, never()).end();

    verify(interrupter).initialize();
    verify(interrupter).execute();

    assertFalse(scheduler.isScheduled(interrupted));
    assertTrue(scheduler.isScheduled(interrupter));
  }
}
