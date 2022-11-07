// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj2.command.Command.InterruptionBehavior;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;

class SchedulingRecursionTest extends CommandTestBase {
  /**
   * <a href="https://github.com/wpilibsuite/allwpilib/issues/4259">wpilibsuite/allwpilib#4259</a>.
   */
  @EnumSource(InterruptionBehavior.class)
  @ParameterizedTest
  void cancelFromInitialize(InterruptionBehavior interruptionBehavior) {
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

            @Override
            public InterruptionBehavior getInterruptionBehavior() {
              return interruptionBehavior;
            }
          };
      Command other = new RunCommand(() -> hasOtherRun.set(true), requirement);

      assertDoesNotThrow(
          () -> {
            scheduler.schedule(selfCancels);
            scheduler.run();
            scheduler.schedule(other);
          });
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
      scheduler.run();
      assertTrue(hasOtherRun.get());
    }
  }

  @EnumSource(InterruptionBehavior.class)
  @ParameterizedTest
  void defaultCommandGetsRescheduledAfterSelfCanceling(InterruptionBehavior interruptionBehavior) {
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

            @Override
            public InterruptionBehavior getInterruptionBehavior() {
              return interruptionBehavior;
            }
          };
      Command other = new RunCommand(() -> hasOtherRun.set(true), requirement);
      scheduler.setDefaultCommand(requirement, other);

      assertDoesNotThrow(
          () -> {
            scheduler.schedule(selfCancels);
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
  @EnumSource(InterruptionBehavior.class)
  void scheduleInitializeFromDefaultCommand(InterruptionBehavior interruptionBehavior) {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new SubsystemBase() {};
      Command other =
          new InstantCommand(() -> {}, requirement).withInterruptBehavior(interruptionBehavior);
      Command defaultCommand =
          new CommandBase() {
            {
              addRequirements(requirement);
            }

            @Override
            public void initialize() {
              counter.incrementAndGet();
              scheduler.schedule(other);
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
