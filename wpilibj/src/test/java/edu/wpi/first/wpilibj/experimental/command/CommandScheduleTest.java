package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;


class CommandScheduleTest extends CommandTestBase {

  @Test
  void instantScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    holder.setFinished(true);
    Command mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);
    assertTrue(scheduler.isScheduled(mockCommand));
    verify(mockCommand).initialize();

    scheduler.run();

    verify(mockCommand).execute();
    verify(mockCommand).end(false);

    assertFalse(scheduler.isScheduled(mockCommand));
  }

  @Test
  void singleIterationScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    Command mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    assertTrue(scheduler.isScheduled(mockCommand));

    scheduler.run();
    holder.setFinished(true);
    scheduler.run();

    verify(mockCommand).initialize();
    verify(mockCommand, times(2)).execute();
    verify(mockCommand).end(false);

    assertFalse(scheduler.isScheduled(mockCommand));
  }

  @Test
  void multiScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command3 = command3Holder.getMock();

    scheduler.scheduleCommands(true, command1, command2, command3);
    assertTrue(scheduler.isScheduled(command1, command2, command3));
    scheduler.run();
    assertTrue(scheduler.isScheduled(command1, command2, command3));

    command1Holder.setFinished(true);
    scheduler.run();
    assertTrue(scheduler.isScheduled(command2, command3));
    assertFalse(scheduler.isScheduled(command1));

    command2Holder.setFinished(true);
    scheduler.run();
    assertTrue(scheduler.isScheduled(command3));
    assertFalse(scheduler.isScheduled(command1, command2));

    command3Holder.setFinished(true);
    scheduler.run();
    assertFalse(scheduler.isScheduled(command1, command2, command3));
  }

  @Test
  void schedulerCancelTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    Command mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    scheduler.run();
    scheduler.cancelCommand(mockCommand);
    scheduler.run();

    verify(mockCommand).execute();
    verify(mockCommand).end(true);
    verify(mockCommand, never()).end(false);

    assertFalse(scheduler.isScheduled(mockCommand));
  }

  @Test
  void notScheduledCancelTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    Command mockCommand = holder.getMock();

    assertDoesNotThrow(() -> scheduler.cancelCommand(mockCommand));
  }
}
