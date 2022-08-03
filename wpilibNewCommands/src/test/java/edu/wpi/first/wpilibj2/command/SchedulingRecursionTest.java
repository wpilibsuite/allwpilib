// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class SchedulingRecursionTest extends CommandTestBase {
  /**
   * <a href="https://github.com/wpilibsuite/allwpilib/issues/4259">wpilibsuite/allwpilib#4259</a>.
   */
  @ValueSource(booleans = {true, false})
  @ParameterizedTest
  void cancelFromInitialize(boolean interruptible) {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean hasOtherRun = new AtomicBoolean();
      Subsystem requirement = new SubsystemBase() {};
      Command selfCancels =
          new CommandBase() {
            {
              addRequirements(requirement);
            }

            @Override
            public void initialize() {
              scheduler.cancel(this);
            }
          };
      Command other = new RunCommand(() -> hasOtherRun.set(true), requirement);

      assertDoesNotThrow(
          () -> {
            scheduler.schedule(interruptible, selfCancels);
            scheduler.run();
            // interruptibility of new arrival isn't checked
            scheduler.schedule(other);
          });
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
      scheduler.run();
      assertTrue(hasOtherRun.get());
    }
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest
  void defaultCommand(boolean interruptible) {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean hasOtherRun = new AtomicBoolean();
      Subsystem requirement = new SubsystemBase() {};
      Command selfCancels =
          new CommandBase() {
            {
              addRequirements(requirement);
            }

            @Override
            public void initialize() {
              scheduler.cancel(this);
            }
          };
      Command other = new RunCommand(() -> hasOtherRun.set(true), requirement);
      scheduler.setDefaultCommand(requirement, other);

      assertDoesNotThrow(
          () -> {
            scheduler.schedule(interruptible, selfCancels);
            scheduler.run();
          });
      scheduler.run();
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
      scheduler.run();
      assertTrue(hasOtherRun.get());
    }
  }

  @Test
  void cancelFromEnd() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Command selfCancels =
          new CommandBase() {
            @Override
            public void end(boolean interrupted) {
              counter.incrementAndGet();
              scheduler.cancel(this);
            }
          };
      scheduler.schedule(selfCancels);

      assertDoesNotThrow(() -> scheduler.cancel(selfCancels));
      assertEquals(1, counter.get());
      assertFalse(scheduler.isScheduled(selfCancels));
    }
  }

  @Test
  void scheduleFromEndCancel() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new SubsystemBase() {};
      InstantCommand other = new InstantCommand(() -> {}, requirement);
      Command selfCancels =
          new CommandBase() {
            {
              addRequirements(requirement);
            }

            @Override
            public void end(boolean interrupted) {
              counter.incrementAndGet();
              scheduler.schedule(other);
            }
          };

      scheduler.schedule(selfCancels);

      assertDoesNotThrow(() -> scheduler.cancel(selfCancels));
      assertEquals(1, counter.get());
      assertFalse(scheduler.isScheduled(selfCancels));
    }
  }

  @Test
  void scheduleFromEndInterrupt() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new SubsystemBase() {};
      InstantCommand other = new InstantCommand(() -> {}, requirement);
      Command selfCancels =
          new CommandBase() {
            {
              addRequirements(requirement);
            }

            @Override
            public void end(boolean interrupted) {
              counter.incrementAndGet();
              scheduler.schedule(other);
            }
          };

      scheduler.schedule(selfCancels);

      assertDoesNotThrow(() -> scheduler.schedule(other));
      assertEquals(1, counter.get());
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
    }
  }

  @ParameterizedTest
  @ValueSource(booleans = {true, false})
  void scheduleInitializeFromDefaultCommand(boolean interruptible) {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new SubsystemBase() {};
      Command other = new InstantCommand(() -> {}, requirement);
      Command defaultCommand =
          new CommandBase() {
            {
              addRequirements(requirement);
            }

            @Override
            public void initialize() {
              counter.incrementAndGet();
              scheduler.schedule(interruptible, other);
            }
          };

      scheduler.setDefaultCommand(requirement, defaultCommand);

      scheduler.run();
      scheduler.run();
      scheduler.run();
      assertEquals(3, counter.get());
      assertFalse(scheduler.isScheduled(defaultCommand));
      assertTrue(scheduler.isScheduled(other));
    }
  }
}
