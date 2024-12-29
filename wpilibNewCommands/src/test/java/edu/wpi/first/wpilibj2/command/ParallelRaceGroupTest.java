// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.reset;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

class ParallelRaceGroupTest extends MultiCompositionTestBase<ParallelRaceGroup> {
  @Test
  void parallelRaceScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      Command group = new ParallelRaceGroup(command1, command2);

      scheduler.schedule(group);

      verify(command1).initialize();
      verify(command2).initialize();

      command1Holder.setFinished(true);
      scheduler.run();
      command2Holder.setFinished(true);
      scheduler.run();

      verify(command1).execute();
      verify(command1).end(false);
      verify(command2).execute();
      verify(command2).end(true);
      verify(command2, never()).end(false);

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void parallelRaceInterruptTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      Command group = new ParallelRaceGroup(command1, command2);

      scheduler.schedule(group);

      scheduler.run();
      scheduler.run();
      scheduler.cancel(group);

      verify(command1, times(2)).execute();
      verify(command1, never()).end(false);
      verify(command1).end(true);

      verify(command2, times(2)).execute();
      verify(command2, never()).end(false);
      verify(command2).end(true);

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void notScheduledCancelTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      Command group = new ParallelRaceGroup(command1, command2);

      assertDoesNotThrow(() -> scheduler.cancel(group));
    }
  }

  @Test
  void parallelRaceRequirementTest() {
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

      Command group = new ParallelRaceGroup(command1, command2);

      scheduler.schedule(group);
      scheduler.schedule(command3);

      assertFalse(scheduler.isScheduled(group));
      assertTrue(scheduler.isScheduled(command3));
    }
  }

  @Test
  void parallelRaceRequirementErrorTest() {
    Subsystem system1 = new SubsystemBase() {};
    Subsystem system2 = new SubsystemBase() {};
    Subsystem system3 = new SubsystemBase() {};

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system2, system3);
    Command command2 = command2Holder.getMock();

    assertThrows(IllegalArgumentException.class, () -> new ParallelRaceGroup(command1, command2));
  }

  @Test
  void parallelRaceOnlyCallsEndOnceTest() {
    Subsystem system1 = new SubsystemBase() {};
    Subsystem system2 = new SubsystemBase() {};

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system2);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();

    Command group1 = Commands.sequence(command1, command2);
    assertNotNull(group1);
    assertNotNull(command3);
    Command group2 = new ParallelRaceGroup(group1, command3);

    try (CommandScheduler scheduler = new CommandScheduler()) {
      scheduler.schedule(group2);
      scheduler.run();
      command1Holder.setFinished(true);
      scheduler.run();
      command2Holder.setFinished(true);
      // at this point the sequential group should be done
      assertDoesNotThrow(scheduler::run);
      assertFalse(scheduler.isScheduled(group2));
    }
  }

  @Test
  void parallelRaceScheduleTwiceTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      Command group = new ParallelRaceGroup(command1, command2);

      scheduler.schedule(group);

      verify(command1).initialize();
      verify(command2).initialize();

      command1Holder.setFinished(true);
      scheduler.run();
      command2Holder.setFinished(true);
      scheduler.run();

      verify(command1).execute();
      verify(command1).end(false);
      verify(command2).execute();
      verify(command2).end(true);
      verify(command2, never()).end(false);

      assertFalse(scheduler.isScheduled(group));

      reset(command1);
      reset(command2);

      scheduler.schedule(group);

      verify(command1).initialize();
      verify(command2).initialize();

      scheduler.run();
      scheduler.run();
      assertTrue(scheduler.isScheduled(group));
      command2Holder.setFinished(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Override
  public ParallelRaceGroup compose(Command... members) {
    return new ParallelRaceGroup(members);
  }
}
