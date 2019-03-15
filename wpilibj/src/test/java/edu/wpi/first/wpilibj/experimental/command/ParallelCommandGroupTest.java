package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

public class ParallelCommandGroupTest extends CommandTestBase {

  @Test
  void parallelGroupScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();
    
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();

    Command group = new ParallelCommandGroup(command1, command2);

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
  void parallelGroupInterruptTest() {
    CommandScheduler scheduler = new CommandScheduler();
    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();

    Command group = new ParallelCommandGroup(command1, command2);

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

  @Test
  void notScheduledCancelTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, new Subsystem[0]);
    Command command2 = command2Holder.getMock();

    Command group = new ParallelCommandGroup(command1, command2);

    assertDoesNotThrow(() -> scheduler.cancelCommand(group));
  }

}
