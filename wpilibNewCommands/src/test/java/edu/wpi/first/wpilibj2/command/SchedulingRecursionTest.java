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
    AtomicBoolean hasOtherRun = new AtomicBoolean();
    Subsystem requirement = new SubsystemBase() {};
    Command selfCancels =
        new CommandBase() {
          {
            addRequirements(requirement);
          }

          @Override
          public void initialize() {
            this.cancel();
          }

          @Override
          public InterruptionBehavior getInterruptionBehavior() {
            return interruptionBehavior;
          }
        };
    Command other = new RunCommand(() -> hasOtherRun.set(true), requirement);

    assertDoesNotThrow(
        () -> {
          selfCancels.schedule();
          CommandScheduler.getInstance().run();
          other.schedule();
        });
    assertFalse(selfCancels.isScheduled());
    assertTrue(other.isScheduled());
    CommandScheduler.getInstance().run();
    assertTrue(hasOtherRun.get());
  }

  @EnumSource(InterruptionBehavior.class)
  @ParameterizedTest
  void defaultCommandGetsRescheduledAfterSelfCanceling(InterruptionBehavior interruptionBehavior) {
    AtomicBoolean hasOtherRun = new AtomicBoolean();
    Subsystem requirement = new SubsystemBase() {};
    Command selfCancels =
        new CommandBase() {
          {
            addRequirements(requirement);
          }

          @Override
          public void initialize() {
            this.cancel();
          }

          @Override
          public InterruptionBehavior getInterruptionBehavior() {
            return interruptionBehavior;
          }
        };
    Command other = new RunCommand(() -> hasOtherRun.set(true), requirement);
    requirement.setDefaultCommand(other);

    assertDoesNotThrow(
        () -> {
          selfCancels.schedule();
          CommandScheduler.getInstance().run();
        });
    CommandScheduler.getInstance().run();
    assertFalse(selfCancels.isScheduled());
    assertTrue(other.isScheduled());
    CommandScheduler.getInstance().run();
    assertTrue(hasOtherRun.get());
  }

  @Test
  void cancelFromEnd() {
    AtomicInteger counter = new AtomicInteger();
    Command selfCancels =
        new CommandBase() {
          @Override
          public void end(boolean interrupted) {
            counter.incrementAndGet();
            this.cancel();
          }
        };
    selfCancels.schedule();

    assertDoesNotThrow(selfCancels::cancel);
    assertEquals(1, counter.get());
    assertFalse(selfCancels.isScheduled());
  }

  @Test
  void scheduleFromEndCancel() {
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
            other.schedule();
          }
        };

    selfCancels.schedule();

    assertDoesNotThrow(selfCancels::cancel);
    assertEquals(1, counter.get());
    assertFalse(selfCancels.isScheduled());
  }

  @Test
  void scheduleFromEndInterrupt() {
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
            other.schedule();
          }
        };

    selfCancels.schedule();

    assertDoesNotThrow(other::schedule);
    assertEquals(1, counter.get());
    assertFalse(selfCancels.isScheduled());
    assertTrue(other.isScheduled());
  }

  @ParameterizedTest
  @EnumSource(InterruptionBehavior.class)
  void scheduleInitializeFromDefaultCommand(InterruptionBehavior interruptionBehavior) {
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
            other.schedule();
          }
        };

    requirement.setDefaultCommand(defaultCommand);

    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    assertEquals(3, counter.get());
    assertFalse(defaultCommand.isScheduled());
    assertTrue(other.isScheduled());
  }
}
