// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class CommandDecoratorTest extends CommandTestBase {
  @Test
  @ResourceLock("timing")
  void withTimeoutTest() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command1 = new WaitCommand(10);

      Command timeout = command1.withTimeout(2);

      scheduler.schedule(timeout);
      scheduler.run();

      assertFalse(scheduler.isScheduled(command1));
      assertTrue(scheduler.isScheduled(timeout));

      SimHooks.stepTiming(3);
      scheduler.run();

      assertFalse(scheduler.isScheduled(timeout));
    } finally {
      SimHooks.resumeTiming();
    }
  }

  @Test
  void untilTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean();

      Command command = new WaitCommand(10).until(condition::get);

      scheduler.schedule(command);
      scheduler.run();
      assertTrue(scheduler.isScheduled(command));
      condition.set(true);
      scheduler.run();
      assertFalse(scheduler.isScheduled(command));
    }
  }

  @Test
  void ignoringDisableTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      var command = new RunCommand(() -> {}).ignoringDisable(true);

      setDSEnabled(false);

      scheduler.schedule(command);

      scheduler.run();
      assertTrue(scheduler.isScheduled(command));
    }
  }

  @Test
  void beforeStartingTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean();
      condition.set(false);

      Command command = new InstantCommand();

      scheduler.schedule(command.beforeStarting(() -> condition.set(true)));

      assertTrue(condition.get());
    }
  }

  @Test
  void andThenLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean();
      condition.set(false);

      Command command = new InstantCommand();

      scheduler.schedule(command.andThen(() -> condition.set(true)));

      assertFalse(condition.get());

      scheduler.run();

      assertTrue(condition.get());
    }
  }

  @Test
  void andThenTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean();
      condition.set(false);

      Command command1 = new InstantCommand();
      Command command2 = new InstantCommand(() -> condition.set(true));

      scheduler.schedule(command1.andThen(command2));

      assertFalse(condition.get());

      scheduler.run();

      assertTrue(condition.get());
    }
  }

  @Test
  void deadlineWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean();
      condition.set(false);

      Command dictator = new WaitUntilCommand(condition::get);
      Command endsBefore = new InstantCommand();
      Command endsAfter = new WaitUntilCommand(() -> false);

      Command group = dictator.deadlineWith(endsBefore, endsAfter);

      scheduler.schedule(group);
      scheduler.run();

      assertTrue(scheduler.isScheduled(group));

      condition.set(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void alongWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean condition = new AtomicBoolean();
      condition.set(false);

      Command command1 = new WaitUntilCommand(condition::get);
      Command command2 = new InstantCommand();

      Command group = command1.alongWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertTrue(scheduler.isScheduled(group));

      condition.set(true);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @Test
  void raceWithTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command1 = new WaitUntilCommand(() -> false);
      Command command2 = new InstantCommand();

      Command group = command1.raceWith(command2);

      scheduler.schedule(group);
      scheduler.run();

      assertFalse(scheduler.isScheduled(group));
    }
  }

  @SuppressWarnings("removal") // Command.perpetually()
  @Test
  void perpetuallyTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command = new InstantCommand();

      Command perpetual = command.perpetually();

      scheduler.schedule(perpetual);
      scheduler.run();
      scheduler.run();
      scheduler.run();

      assertTrue(scheduler.isScheduled(perpetual));
    }
  }

  @Test
  void endlesslyTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Command command = new InstantCommand();

      Command perpetual = command.endlessly();

      scheduler.schedule(perpetual);
      scheduler.run();
      scheduler.run();
      scheduler.run();

      assertTrue(scheduler.isScheduled(perpetual));
    }
  }

  @Test
  void unlessTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean unlessCondition = new AtomicBoolean(true);
      AtomicBoolean hasRunCondition = new AtomicBoolean(false);

      Command command =
          new InstantCommand(() -> hasRunCondition.set(true)).unless(unlessCondition::get);

      scheduler.schedule(command);
      scheduler.run();
      assertFalse(hasRunCondition.get());

      unlessCondition.set(false);
      scheduler.schedule(command);
      scheduler.run();
      assertTrue(hasRunCondition.get());
    }
  }
}
