// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.internal.verification.VerificationModeFactory.times;

import java.util.Arrays;
import org.junit.jupiter.api.Test;

class ParallelDeadlineGroupTest extends MultiCompositionTestBase<ParallelDeadlineGroup> {
  @Test
  void parallelDeadlineScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      command2Holder.setFinished(true);
      MockCommandHolder command3Holder = new MockCommandHolder(true);
      Command command3 = command3Holder.getMock();

      Command group = new ParallelDeadlineGroup(command1, command2, command3);

      scheduler.schedule(group);
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
  }

  @Test
  void parallelDeadlineInterruptTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      command2Holder.setFinished(true);

      Command group = new ParallelDeadlineGroup(command1, command2);

      scheduler.schedule(group);

      scheduler.run();
      scheduler.run();
      scheduler.cancel(group);

      verify(command1, times(2)).execute();
      verify(command1, never()).end(false);
      verify(command1).end(true);

      verify(command2).execute();
      verify(command2).end(false);
      verify(command2, never()).end(true);

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void parallelDeadlineRequirementTest() {
    Subsystem system1 = new SubsystemBase() {};
    Subsystem system2 = new SubsystemBase() {};
    Subsystem system3 = new SubsystemBase() {};
    Subsystem system4 = new SubsystemBase() {};

    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
      Command command3 = command3Holder.getMock();

      Command group = new ParallelDeadlineGroup(command1, command2);

      scheduler.schedule(group);
      scheduler.schedule(command3);

      assertFalse(scheduler.isScheduled(group));
      assertTrue(scheduler.isScheduled(command3));
    }
  }

  @Test
  void parallelDeadlineRequirementErrorTest() {
    Subsystem system1 = new SubsystemBase() {};
    Subsystem system2 = new SubsystemBase() {};
    Subsystem system3 = new SubsystemBase() {};

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system2, system3);
    Command command2 = command2Holder.getMock();

    assertThrows(
        IllegalArgumentException.class, () -> new ParallelDeadlineGroup(command1, command2));
  }

  @Test
  void parallelDeadlineSetDeadlineToDeadlineTest() {
    Command a = Commands.none();
    ParallelDeadlineGroup group = new ParallelDeadlineGroup(a);
    assertDoesNotThrow(() -> group.setDeadline(a));
  }

  @Test
  void parallelDeadlineSetDeadlineDuplicateTest() {
    Command a = Commands.none();
    Command b = Commands.none();
    ParallelDeadlineGroup group = new ParallelDeadlineGroup(a, b);
    assertThrows(IllegalArgumentException.class, () -> group.setDeadline(b));
  }

  @Override
  public ParallelDeadlineGroup compose(Command... members) {
    return new ParallelDeadlineGroup(members[0], Arrays.copyOfRange(members, 1, members.length));
  }
}
