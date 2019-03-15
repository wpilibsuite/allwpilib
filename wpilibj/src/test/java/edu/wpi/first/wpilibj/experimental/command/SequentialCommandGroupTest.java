package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

public class SequentialCommandGroupTest extends CommandTestBase {
  @Test
  void sequentialGroupScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2);

    scheduler.scheduleCommand(group, true);

    verify(command1).initialize();
    verify(command2, never()).initialize();

    command1Holder.setFinished(true);
    scheduler.run();

    verify(command1).execute();
    verify(command1).end();
    verify(command2).initialize();
    verify(command2, never()).execute();
    verify(command2, never()).end();

    command2Holder.setFinished(true);
    scheduler.run();

    verify(command1).execute();
    verify(command1).end();
    verify(command2).execute();
    verify(command2).end();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void sequentialGroupInterruptTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command3 = command3Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2, command3);

    scheduler.scheduleCommand(group, true);

    command1Holder.setFinished(true);
    scheduler.run();
    scheduler.cancelCommand(group);
    scheduler.run();

    verify(command1).execute();
    verify(command1, never()).interrupted();
    verify(command1).end();
    verify(command2, never()).execute();
    verify(command2).interrupted();
    verify(command2, never()).end();
    verify(command3, never()).initialize();
    verify(command3, never()).execute();
    verify(command3, never()).interrupted();
    verify(command3, never()).end();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void notScheduledCancelTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2);

    scheduler.cancelCommand(group);
  }
}
