package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
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
    verify(mockCommand).end();

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
    verify(mockCommand).end();

    assertFalse(scheduler.isScheduled(mockCommand));
  }

  @Test
  void cancelTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    Command mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    scheduler.run();
    scheduler.cancelCommand(mockCommand);
    scheduler.run();

    verify(mockCommand).execute();
    verify(mockCommand).interrupted();
    verify(mockCommand, never()).end();

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
