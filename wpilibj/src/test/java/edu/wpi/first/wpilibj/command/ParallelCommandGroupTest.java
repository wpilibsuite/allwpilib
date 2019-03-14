package edu.wpi.first.wpilibj.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.*;
import static org.mockito.Mockito.*;

public class ParallelCommandGroupTest extends ICommandTestBase {

  @Test
  void parallelCommandScheduleTest() {
    SchedulerNew scheduler = new SchedulerNew();
    
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command2 = command2Holder.getMock();

    ICommand group = new ParallelCommandGroup(new ICommand[]{command1, command2});

    scheduler.scheduleCommand(group, true);

    verify(command1).initialize();
    verify(command2).initialize();

    command1Holder.setFinished(true);
    scheduler.run();
    command2Holder.setFinished(true);
    scheduler.run();

    verify(command1).execute();
    verify(command1).end();
    verify(command2, times(2)).execute();
    verify(command2).end();

    assertFalse(scheduler.isScheduled(group));
  }

  @Test
  void parallelCommandInterruptTest() {
    SchedulerNew scheduler = new SchedulerNew();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    ICommand command2 = command2Holder.getMock();

    ICommand group = new ParallelCommandGroup(new ICommand[]{command1, command2});

    scheduler.scheduleCommand(group, true);

    command1Holder.setFinished(true);
    scheduler.run();
    scheduler.run();
    scheduler.cancelCommand(group);

    verify(command1).execute();
    verify(command1).end();
    verify(command1, never()).interrupted();

    verify(command2, times(2)).execute();
    verify(command2, never()).end();
    verify(command2).interrupted();

    assertFalse(scheduler.isScheduled(group));
  }
}
