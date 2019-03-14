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

  @Test
  void requirementUninterruptibleTest() {
    SchedulerNew scheduler = new SchedulerNew();

    Subsystem requirement = new ASubsystem();

    MockCommandHolder interruptedHolder = new MockCommandHolder(true, requirement);
    ICommand notInterrupted = interruptedHolder.getMock();
    MockCommandHolder interrupterHolder = new MockCommandHolder(true, requirement);
    ICommand interrupter = interrupterHolder.getMock();

    scheduler.scheduleCommand(notInterrupted, false);
    scheduler.scheduleCommand(interrupter, true);

    assertTrue(scheduler.isScheduled(notInterrupted));
    assertFalse(scheduler.isScheduled(interrupter));
  }

  @Test
  void parallelGroupRequirementTest() {
    Subsystem system1 = new ASubsystem();
    Subsystem system2 = new ASubsystem();
    Subsystem system3 = new ASubsystem();
    Subsystem system4 = new ASubsystem();

    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    ICommand command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    ICommand command3 = command3Holder.getMock();

    ICommand group = new ParallelCommandGroup(command1, command2);

    scheduler.scheduleCommand(group, true);
    scheduler.scheduleCommand(command3, true);

    assertFalse(scheduler.isScheduled(group));
    assertTrue(scheduler.isScheduled(command3));
  }

  @Test
  void sequentialGroupRequirementTest() {
    Subsystem system1 = new ASubsystem();
    Subsystem system2 = new ASubsystem();
    Subsystem system3 = new ASubsystem();
    Subsystem system4 = new ASubsystem();

    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    ICommand command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    ICommand command3 = command3Holder.getMock();

    ICommand group = new SequentialCommandGroup(command1, command2);

    scheduler.scheduleCommand(group, true);
    scheduler.scheduleCommand(command3, true);

    assertFalse(scheduler.isScheduled(group));
    assertTrue(scheduler.isScheduled(command3));
  }
}
