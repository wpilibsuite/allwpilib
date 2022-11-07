// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static edu.wpi.first.wpilibj2.command.Commands.parallel;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Map;
import org.junit.jupiter.api.Test;

class RobotDisabledCommandTest extends CommandTestBase {
  @Test
  void robotDisabledCommandCancelTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(false);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);

      assertTrue(scheduler.isScheduled(mockCommand));

      setDSEnabled(false);

      scheduler.run();

      assertFalse(scheduler.isScheduled(mockCommand));

      setDSEnabled(true);
    }
  }

  @Test
  void runWhenDisabledTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder holder = new MockCommandHolder(true);
      Command mockCommand = holder.getMock();

      scheduler.schedule(mockCommand);

      assertTrue(scheduler.isScheduled(mockCommand));

      setDSEnabled(false);

      scheduler.run();

      assertTrue(scheduler.isScheduled(mockCommand));
    }
  }

  @Test
  void sequentialGroupRunWhenDisabledTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true);
      Command command3 = command3Holder.getMock();
      MockCommandHolder command4Holder = new MockCommandHolder(false);
      Command command4 = command4Holder.getMock();

      Command runWhenDisabled = new SequentialCommandGroup(command1, command2);
      Command dontRunWhenDisabled = new SequentialCommandGroup(command3, command4);

      scheduler.schedule(runWhenDisabled);
      scheduler.schedule(dontRunWhenDisabled);

      setDSEnabled(false);

      scheduler.run();

      assertTrue(scheduler.isScheduled(runWhenDisabled));
      assertFalse(scheduler.isScheduled(dontRunWhenDisabled));
    }
  }

  @Test
  void parallelGroupRunWhenDisabledTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();
      MockCommandHolder command3Holder = new MockCommandHolder(true);
      Command command3 = command3Holder.getMock();
      MockCommandHolder command4Holder = new MockCommandHolder(false);
      Command command4 = command4Holder.getMock();

      Command runWhenDisabled = new ParallelCommandGroup(command1, command2);
      Command dontRunWhenDisabled = new ParallelCommandGroup(command3, command4);

      scheduler.schedule(runWhenDisabled);
      scheduler.schedule(dontRunWhenDisabled);

      setDSEnabled(false);

      scheduler.run();

      assertTrue(scheduler.isScheduled(runWhenDisabled));
      assertFalse(scheduler.isScheduled(dontRunWhenDisabled));
    }
  }

  @Test
  void conditionalRunWhenDisabledTest() {
    setDSEnabled(false);

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();
    MockCommandHolder command4Holder = new MockCommandHolder(false);
    Command command4 = command4Holder.getMock();

    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command runWhenDisabled = new ConditionalCommand(command1, command2, () -> true);
      Command dontRunWhenDisabled = new ConditionalCommand(command3, command4, () -> true);

      scheduler.schedule(runWhenDisabled, dontRunWhenDisabled);

      assertTrue(scheduler.isScheduled(runWhenDisabled));
      assertFalse(scheduler.isScheduled(dontRunWhenDisabled));
    }
  }

  @Test
  void selectRunWhenDisabledTest() {
    setDSEnabled(false);

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();
    MockCommandHolder command4Holder = new MockCommandHolder(false);
    Command command4 = command4Holder.getMock();

    Command runWhenDisabled = new SelectCommand(Map.of(1, command1, 2, command2), () -> 1);
    Command dontRunWhenDisabled = new SelectCommand(Map.of(1, command3, 2, command4), () -> 1);

    try (CommandScheduler scheduler = new CommandScheduler()) {
      scheduler.schedule(runWhenDisabled, dontRunWhenDisabled);

      assertTrue(scheduler.isScheduled(runWhenDisabled));
      assertFalse(scheduler.isScheduled(dontRunWhenDisabled));
    }
  }

  @Test
  void parallelConditionalRunWhenDisabledTest() {
    setDSEnabled(false);

    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    MockCommandHolder command2Holder = new MockCommandHolder(true);
    Command command2 = command2Holder.getMock();
    MockCommandHolder command3Holder = new MockCommandHolder(true);
    Command command3 = command3Holder.getMock();
    MockCommandHolder command4Holder = new MockCommandHolder(false);
    Command command4 = command4Holder.getMock();

    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command runWhenDisabled = new ConditionalCommand(command1, command2, () -> true);
      Command dontRunWhenDisabled = new ConditionalCommand(command3, command4, () -> true);

      Command parallel = parallel(runWhenDisabled, dontRunWhenDisabled);

      scheduler.schedule(parallel);

      assertFalse(scheduler.isScheduled(runWhenDisabled));
    }
  }
}
