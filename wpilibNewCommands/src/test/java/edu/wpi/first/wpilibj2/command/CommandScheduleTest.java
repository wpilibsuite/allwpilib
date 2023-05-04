// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.times;
import static org.mockito.Mockito.verify;

import org.junit.jupiter.api.Test;

class CommandScheduleTest extends CommandTestBase {
  @Test
  void instantScheduleTest() {
    MockCommandHolder holder = new MockCommandHolder(true);
    holder.setFinished(true);
    Command mockCommand = holder.getMock();

    mockCommand.schedule();
    assertTrue(mockCommand.isScheduled());
    verify(mockCommand).initialize();

    CommandScheduler.getInstance().run();

    verify(mockCommand).execute();
    verify(mockCommand).end(false);

    assertFalse(mockCommand.isScheduled());
  }

  @Test
  void singleIterationScheduleTest() {
    MockCommandHolder holder = new MockCommandHolder(true);
    Command mockCommand = holder.getMock();

    mockCommand.schedule();

    assertTrue(mockCommand.isScheduled());

    CommandScheduler.getInstance().run();
    holder.setFinished(true);
    CommandScheduler.getInstance().run();

    verify(mockCommand).initialize();
    verify(mockCommand, times(2)).execute();
    verify(mockCommand).end(false);

    assertFalse(mockCommand.isScheduled());
  }

  @Test
  void multiScheduleTest() {
    CommandScheduler scheduler = CommandScheduler.getInstance();
    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();

    scheduler.schedule(command1, command2, command3);
    assertTrue(scheduler.isScheduled(command1, command2, command3));
    CommandScheduler.getInstance().run();
    assertTrue(scheduler.isScheduled(command1, command2, command3));

    command1Holder.setFinished(true);
    CommandScheduler.getInstance().run();
    assertTrue(scheduler.isScheduled(command2, command3));
    assertFalse(command1.isScheduled());

    command2Holder.setFinished(true);
    CommandScheduler.getInstance().run();
    assertTrue(command3.isScheduled());
    assertFalse(scheduler.isScheduled(command1, command2));

    command3Holder.setFinished(true);
    CommandScheduler.getInstance().run();
    assertFalse(scheduler.isScheduled(command1, command2, command3));
  }

  @Test
  void schedulerCancelTest() {
    MockCommandHolder holder = new MockCommandHolder(true);
    Command mockCommand = holder.getMock();

    mockCommand.schedule();

    CommandScheduler.getInstance().run();
    mockCommand.cancel();
    CommandScheduler.getInstance().run();

    verify(mockCommand).execute();
    verify(mockCommand).end(true);
    verify(mockCommand, never()).end(false);

    assertFalse(mockCommand.isScheduled());
  }

  @Test
  void notScheduledCancelTest() {
    MockCommandHolder holder = new MockCommandHolder(true);
    Command mockCommand = holder.getMock();

    assertDoesNotThrow(mockCommand::cancel);
  }
}
