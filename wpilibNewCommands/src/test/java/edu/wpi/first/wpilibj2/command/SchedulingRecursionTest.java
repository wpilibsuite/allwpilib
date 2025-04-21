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
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new SubsystemBase() {};
      Command selfCancels =
          new Command() {
            {
              addRequirements(requirement);
            }

            @Override
            public void initialize() {
              scheduler.cancel(this);
            }

            @Override
            public void end(boolean interrupted) {
              counter.incrementAndGet();
            }

            @Override
            public InterruptionBehavior getInterruptionBehavior() {
              return interruptionBehavior;
            }
          };
      Command other = requirement.run(() -> hasOtherRun.set(true));

      assertDoesNotThrow(
          () -> {
            scheduler.schedule(selfCancels);
            scheduler.run();
            scheduler.schedule(other);
          });
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
      assertEquals(1, counter.get());
      scheduler.run();
      assertTrue(hasOtherRun.get());
    }
  }

  @EnumSource(InterruptionBehavior.class)
  @ParameterizedTest
  void cancelFromInitializeAction(InterruptionBehavior interruptionBehavior) {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean hasOtherRun = new AtomicBoolean();
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new Subsystem() {};
      Command selfCancels =
          new Command() {
            {
              addRequirements(requirement);
            }

            @Override
            public void end(boolean interrupted) {
              counter.incrementAndGet();
            }

            @Override
            public InterruptionBehavior getInterruptionBehavior() {
              return interruptionBehavior;
            }
          };
      Command other = requirement.run(() -> hasOtherRun.set(true));

      assertDoesNotThrow(
          () -> {
            scheduler.onCommandInitialize(cmd -> scheduler.cancel(selfCancels));
            scheduler.schedule(selfCancels);
            scheduler.run();
            scheduler.schedule(other);
          });
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
      assertEquals(1, counter.get());
      scheduler.run();
      assertTrue(hasOtherRun.get());
    }
  }

  @EnumSource(InterruptionBehavior.class)
  @ParameterizedTest
  void defaultCommandGetsRescheduledAfterSelfCanceling(InterruptionBehavior interruptionBehavior) {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean hasOtherRun = new AtomicBoolean();
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new SubsystemBase() {};
      Command selfCancels =
          new Command() {
            {
              addRequirements(requirement);
            }

            @Override
            public void initialize() {
              scheduler.cancel(this);
            }

            @Override
            public void end(boolean interrupted) {
              counter.incrementAndGet();
            }

            @Override
            public InterruptionBehavior getInterruptionBehavior() {
              return interruptionBehavior;
            }
          };
      Command other = requirement.run(() -> hasOtherRun.set(true));
      scheduler.setDefaultCommand(requirement, other);

      assertDoesNotThrow(
          () -> {
            scheduler.schedule(selfCancels);
            scheduler.run();
          });
      scheduler.run();
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
      assertEquals(1, counter.get());
      scheduler.run();
      assertTrue(hasOtherRun.get());
    }
  }

  @Test
  void cancelFromEnd() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Command selfCancels =
          new Command() {
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
  void cancelFromInterruptAction() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Command selfCancels = Commands.idle();
      scheduler.onCommandInterrupt(
          cmd -> {
            counter.incrementAndGet();
            scheduler.cancel(selfCancels);
          });
      scheduler.schedule(selfCancels);

      assertDoesNotThrow(() -> scheduler.cancel(selfCancels));
      assertEquals(1, counter.get());
      assertFalse(scheduler.isScheduled(selfCancels));
    }
  }

  @Test
  void cancelFromEndLoop() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      FunctionalCommand dCancelsAll =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancelAll();
              },
              () -> true);
      FunctionalCommand cCancelsD =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancel(dCancelsAll);
              },
              () -> true);
      FunctionalCommand bCancelsC =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancel(cCancelsD);
              },
              () -> true);
      FunctionalCommand aCancelsB =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancel(bCancelsC);
              },
              () -> true);

      scheduler.schedule(aCancelsB);
      scheduler.schedule(bCancelsC);
      scheduler.schedule(cCancelsD);
      scheduler.schedule(dCancelsAll);

      assertDoesNotThrow(() -> scheduler.cancel(aCancelsB));
      assertEquals(4, counter.get());
      assertFalse(scheduler.isScheduled(aCancelsB));
      assertFalse(scheduler.isScheduled(bCancelsC));
      assertFalse(scheduler.isScheduled(cCancelsD));
      assertFalse(scheduler.isScheduled(dCancelsAll));
    }
  }

  @Test
  void cancelFromEndLoopWhileInRunLoop() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      FunctionalCommand dCancelsAll =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancelAll();
              },
              () -> true);
      FunctionalCommand cCancelsD =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancel(dCancelsAll);
              },
              () -> true);
      FunctionalCommand bCancelsC =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancel(cCancelsD);
              },
              () -> true);
      FunctionalCommand aCancelsB =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.cancel(bCancelsC);
              },
              () -> true);

      scheduler.schedule(aCancelsB);
      scheduler.schedule(bCancelsC);
      scheduler.schedule(cCancelsD);
      scheduler.schedule(dCancelsAll);

      assertDoesNotThrow(scheduler::run);
      assertEquals(4, counter.get());
      assertFalse(scheduler.isScheduled(aCancelsB));
      assertFalse(scheduler.isScheduled(bCancelsC));
      assertFalse(scheduler.isScheduled(cCancelsD));
      assertFalse(scheduler.isScheduled(dCancelsAll));
    }
  }

  @Test
  void multiCancelFromEnd() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      FunctionalCommand bIncrementsCounter =
          new FunctionalCommand(
              () -> {}, () -> {}, interrupted -> counter.incrementAndGet(), () -> true);
      Command aCancelsB =
          new Command() {
            @Override
            public void end(boolean interrupted) {
              counter.incrementAndGet();
              scheduler.cancel(bIncrementsCounter);
              scheduler.cancel(this);
            }
          };

      scheduler.schedule(aCancelsB);
      scheduler.schedule(bIncrementsCounter);

      assertDoesNotThrow(() -> scheduler.cancel(aCancelsB));
      assertEquals(2, counter.get());
      assertFalse(scheduler.isScheduled(aCancelsB));
      assertFalse(scheduler.isScheduled(bIncrementsCounter));
    }
  }

  @Test
  void scheduleFromEndCancel() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new SubsystemBase() {};
      Command other = requirement.runOnce(() -> {});
      FunctionalCommand selfCancels =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.schedule(other);
              },
              () -> false,
              requirement);

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
      Command other = requirement.runOnce(() -> {});
      FunctionalCommand selfCancels =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.schedule(other);
              },
              () -> false,
              requirement);

      scheduler.schedule(selfCancels);

      assertDoesNotThrow(() -> scheduler.schedule(other));
      assertEquals(1, counter.get());
      assertFalse(scheduler.isScheduled(selfCancels));
      assertTrue(scheduler.isScheduled(other));
    }
  }

  @Test
  void scheduleFromEndInterruptAction() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new Subsystem() {};
      Command other = requirement.runOnce(() -> {});
      Command selfCancels = requirement.runOnce(() -> {});
      scheduler.onCommandInterrupt(
          cmd -> {
            counter.incrementAndGet();
            scheduler.schedule(other);
          });
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
      Command other = requirement.runOnce(() -> {}).withInterruptBehavior(interruptionBehavior);
      FunctionalCommand defaultCommand =
          new FunctionalCommand(
              () -> {
                counter.incrementAndGet();
                scheduler.schedule(other);
              },
              () -> {},
              interrupted -> {},
              () -> false,
              requirement);

      scheduler.setDefaultCommand(requirement, defaultCommand);

      scheduler.run();
      scheduler.run();
      scheduler.run();
      assertEquals(3, counter.get());
      assertFalse(scheduler.isScheduled(defaultCommand));
      assertTrue(scheduler.isScheduled(other));
    }
  }

  @Test
  void cancelDefaultCommandFromEnd() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();
      Subsystem requirement = new Subsystem() {};
      Command defaultCommand =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> counter.incrementAndGet(),
              () -> false,
              requirement);
      Command other = requirement.runOnce(() -> {});
      Command cancelDefaultCommand =
          new FunctionalCommand(
              () -> {},
              () -> {},
              interrupted -> {
                counter.incrementAndGet();
                scheduler.schedule(other);
              },
              () -> false);

      assertDoesNotThrow(
          () -> {
            scheduler.schedule(cancelDefaultCommand);
            scheduler.setDefaultCommand(requirement, defaultCommand);

            scheduler.run();
            scheduler.cancel(cancelDefaultCommand);
          });
      assertEquals(2, counter.get());
      assertFalse(scheduler.isScheduled(defaultCommand));
      assertTrue(scheduler.isScheduled(other));
    }
  }
}
