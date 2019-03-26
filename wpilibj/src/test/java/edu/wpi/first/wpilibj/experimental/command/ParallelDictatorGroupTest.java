package edu.wpi.first.wpilibj.experimental.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.internal.verification.VerificationModeFactory.times;

public class ParallelDictatorGroupTest extends CommandTestBase {
  @Test
  void parallelDictatorScheduleTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    command2Holder.setFinished(true);
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();

    Command group = new ParallelDictatorGroup(command1, command2, command3);

    scheduler.scheduleCommand(group, true);
    scheduler.run();

    assertTrue(scheduler.isScheduled(group));

    command1Holder.setFinished(true);
    scheduler.run();

    assertFalse(scheduler.isScheduled(group));

    verify(command2).initialize();
    verify(command2).execute();
    verify(command2).end(false);
    verify(command2, never()).end(true);

    verify(command1).initialize();
    verify(command1, times(2)).execute();
    verify(command1).end(false);
    verify(command1, never()).end(true);

    verify(command3).initialize();
    verify(command3, times(2)).execute();
    verify(command3, never()).end(false);
    verify(command3).end(true);
  }

  @Test
  void parallelDictatorInterruptTest() {
    CommandScheduler scheduler = new CommandScheduler();

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    command2Holder.setFinished(true);

    Command group = new ParallelDictatorGroup(command1, command2);

    scheduler.scheduleCommand(group, true);

    scheduler.run();
    scheduler.run();
    scheduler.cancelCommand(group);

    verify(command1, times(2)).execute();
    verify(command1, never()).end(false);
    verify(command1).end(true);

    verify(command2).execute();
    verify(command2).end(false);
    verify(command2, never()).end(true);

    assertFalse(scheduler.isScheduled(group));
  }
}
