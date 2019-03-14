package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.*;


class ICommandScheduleTest extends ICommandTestBase {

  @Test
  void instantScheduleTest() {
    SchedulerNew scheduler = new SchedulerNew();
    
    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    holder.setFinished(true);
    ICommand mockCommand = holder.getMock();

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
    SchedulerNew scheduler = new SchedulerNew();
    
    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand mockCommand = holder.getMock();

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
    SchedulerNew scheduler = new SchedulerNew();

    MockCommandHolder holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand mockCommand = holder.getMock();

    scheduler.scheduleCommand(mockCommand, true);

    scheduler.run();
    scheduler.cancelCommand(mockCommand);
    scheduler.run();

    verify(mockCommand).execute();
    verify(mockCommand).interrupted();
    verify(mockCommand, never()).end();

    assertFalse(scheduler.isScheduled(mockCommand));
  }
}
