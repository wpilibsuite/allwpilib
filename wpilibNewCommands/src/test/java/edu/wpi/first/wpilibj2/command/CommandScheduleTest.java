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
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      holder.setFinished(true);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);
      assertTrue(scheduler.isScheduled(mockCommand));
      verify(mockCommand).initialize();

      scheduler.run();

      verify(mockCommand).execute();
      verify(mockCommand).end(false);

      assertFalse(scheduler.isScheduled(mockCommand));
    }
  }

  @Test
  void singleIterationScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);

      assertTrue(scheduler.isScheduled(mockCommand));

      scheduler.run();
      holder.setFinished(true);
      scheduler.run();

      verify(mockCommand).initialize();
      verify(mockCommand, times(2)).execute();
      verify(mockCommand).end(false);

      assertFalse(scheduler.isScheduled(mockCommand));
    }
  }

  @Test
  void multiScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true);
      Command command3 = command3Holder.getMock();

      scheduler.schedule(command1, command2, command3);
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
  }

  @Test
  void schedulerCancelTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);

      scheduler.run();
      scheduler.cancel(mockCommand);
      scheduler.run();

      verify(mockCommand).execute();
      verify(mockCommand).end(true);
      verify(mockCommand, never()).end(false);

      assertFalse(scheduler.isScheduled(mockCommand));
    }
  }

  @Test
  void notScheduledCancelTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();

      assertDoesNotThrow(() -> scheduler.cancel(mockCommand));
    }
  }
}
