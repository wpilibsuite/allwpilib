// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

class SequentialCommandGroupTest extends CommandTestBase
    implements MultiCompositionTestBase<SequentialCommandGroup> {
  @Test
  void sequentialGroupScheduleTest() {
    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2);

    group.schedule();

    verify(command1).initialize();
    verify(command2, never()).initialize();

    command1Holder.setFinished(true);
    CommandScheduler.getInstance().run();

    verify(command1).execute();
    verify(command1).end(false);
    verify(command2).initialize();
    verify(command2, never()).execute();
    verify(command2, never()).end(false);

    command2Holder.setFinished(true);
    CommandScheduler.getInstance().run();

    verify(command1).execute();
    verify(command1).end(false);
    verify(command2).execute();
    verify(command2).end(false);

    assertFalse(group.isScheduled());
  }

  @Test
  void sequentialGroupInterruptTest() {
    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2, command3);

    group.schedule();

    command1Holder.setFinished(true);
    CommandScheduler.getInstance().run();
    group.cancel();
    CommandScheduler.getInstance().run();

    verify(command1).execute();
    verify(command1, never()).end(true);
    verify(command1).end(false);
    verify(command2, never()).execute();
    verify(command2).end(true);
    verify(command2, never()).end(false);
    verify(command3, never()).initialize();
    verify(command3, never()).execute();
    verify(command3, never()).end(true);
    verify(command3, never()).end(false);

    assertFalse(group.isScheduled());
  }

  @Test
  void notScheduledCancelTest() {
    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2);

    assertDoesNotThrow(group::cancel);
  }

  @Test
  void sequentialGroupRequirementTest() {
    Subsystem system1 = new SubsystemBase() {};
    Subsystem system2 = new SubsystemBase() {};
    Subsystem system3 = new SubsystemBase() {};
    Subsystem system4 = new SubsystemBase() {};

    MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true, system3, system4);
    Command command3 = command3Holder.getMock();

    Command group = new SequentialCommandGroup(command1, command2);

    group.schedule();
    command3.schedule();

    assertFalse(group.isScheduled());
    assertTrue(command3.isScheduled());
  }

  @Override
  public SequentialCommandGroup compose(Command... members) {
    return new SequentialCommandGroup(members);
  }
}
