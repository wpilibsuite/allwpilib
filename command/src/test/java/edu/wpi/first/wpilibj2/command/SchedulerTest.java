// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class SchedulerTest extends CommandTestBase {
  @Test
  void schedulerLambdaTestNoInterrupt() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      scheduler.onCommandInitialize(command -> counter.incrementAndGet());
      scheduler.onCommandExecute(command -> counter.incrementAndGet());
      scheduler.onCommandFinish(command -> counter.incrementAndGet());

      scheduler.schedule(Commands.none());
      scheduler.run();

      assertEquals(counter.get(), 3);
    }
  }

  @Test
  void schedulerInterruptLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      scheduler.onCommandInterrupt(command -> counter.incrementAndGet());

      Command command = Commands.idle();

      scheduler.schedule(command);
      scheduler.cancel(command);

      assertEquals(counter.get(), 1);
    }
  }

  @Test
  void schedulerInterruptNoCauseLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      scheduler.onCommandInterrupt(
          (interrupted, cause) -> {
            assertFalse(cause.isPresent());
            counter.incrementAndGet();
          });

      Command command = Commands.run(() -> {});

      scheduler.schedule(command);
      scheduler.cancel(command);

      assertEquals(1, counter.get());
    }
  }

  @Test
  void schedulerInterruptCauseLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      Subsystem subsystem = new Subsystem() {};
      Command command = subsystem.run(() -> {});
      Command interruptor = subsystem.runOnce(() -> {});

      scheduler.onCommandInterrupt(
          (interrupted, cause) -> {
            assertTrue(cause.isPresent());
            assertSame(interruptor, cause.get());
            counter.incrementAndGet();
          });

      scheduler.schedule(command);
      scheduler.schedule(interruptor);

      assertEquals(1, counter.get());
    }
  }

  @Test
  void schedulerInterruptCauseLambdaInRunLoopTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      Subsystem subsystem = new Subsystem() {};
      Command command = subsystem.run(() -> {});
      Command interruptor = subsystem.runOnce(() -> {});
      // This command will schedule interruptor in execute() inside the run loop
      Command interruptorScheduler = Commands.runOnce(() -> scheduler.schedule(interruptor));

      scheduler.onCommandInterrupt(
          (interrupted, cause) -> {
            assertTrue(cause.isPresent());
            assertSame(interruptor, cause.get());
            counter.incrementAndGet();
          });

      scheduler.schedule(command);
      scheduler.schedule(interruptorScheduler);

      scheduler.run();

      assertEquals(1, counter.get());
    }
  }

  @Test
  void registerSubsystemTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger(0);
      Subsystem system =
          new SubsystemBase() {
            @Override
            public void periodic() {
              counter.incrementAndGet();
            }
          };

      assertDoesNotThrow(() -> scheduler.registerSubsystem(system));

      scheduler.run();
      assertEquals(1, counter.get());
    }
  }

  @Test
  void unregisterSubsystemTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger(0);
      Subsystem system =
          new SubsystemBase() {
            @Override
            public void periodic() {
              counter.incrementAndGet();
            }
          };
      scheduler.registerSubsystem(system);
      assertDoesNotThrow(() -> scheduler.unregisterSubsystem(system));

      scheduler.run();
      assertEquals(0, counter.get());
    }
  }

  @Test
  void schedulerCancelAllTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      scheduler.onCommandInterrupt(command -> counter.incrementAndGet());
      scheduler.onCommandInterrupt((command, interruptor) -> assertFalse(interruptor.isPresent()));

      Command command = Commands.idle();
      Command command2 = Commands.idle();

      scheduler.schedule(command);
      scheduler.schedule(command2);
      scheduler.cancelAll();

      assertEquals(counter.get(), 2);
    }
  }

  @Test
  void scheduleScheduledNoOp() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      Command command = Commands.startEnd(counter::incrementAndGet, () -> {});

      scheduler.schedule(command);
      scheduler.schedule(command);

      assertEquals(counter.get(), 1);
    }
  }
}
