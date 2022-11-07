// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;

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

      scheduler.schedule(new InstantCommand());
      scheduler.run();

      assertEquals(counter.get(), 3);
    }
  }

  @Test
  void schedulerInterruptLambdaTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      scheduler.onCommandInterrupt(command -> counter.incrementAndGet());

      Command command = new WaitCommand(10);

      scheduler.schedule(command);
      scheduler.cancel(command);

      assertEquals(counter.get(), 1);
    }
  }

  @Test
  void unregisterSubsystemTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Subsystem system = new SubsystemBase() {};

      scheduler.registerSubsystem(system);
      assertDoesNotThrow(() -> scheduler.unregisterSubsystem(system));
    }
  }

  @Test
  void schedulerCancelAllTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger();

      scheduler.onCommandInterrupt(command -> counter.incrementAndGet());

      Command command = new WaitCommand(10);
      Command command2 = new WaitCommand(10);

      scheduler.schedule(command);
      scheduler.schedule(command2);
      scheduler.cancelAll();

      assertEquals(counter.get(), 2);
    }
  }
}
