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
    AtomicInteger counter = new AtomicInteger();

    CommandScheduler.getInstance().onCommandInitialize(command -> counter.incrementAndGet());
    CommandScheduler.getInstance().onCommandExecute(command -> counter.incrementAndGet());
    CommandScheduler.getInstance().onCommandFinish(command -> counter.incrementAndGet());

    new InstantCommand().schedule();
    CommandScheduler.getInstance().run();

    assertEquals(counter.get(), 3);
  }

  @Test
  void schedulerInterruptLambdaTest() {
    AtomicInteger counter = new AtomicInteger();

    CommandScheduler.getInstance().onCommandInterrupt(command -> counter.incrementAndGet());

    Command command = new WaitCommand(10);

    command.schedule();
    command.cancel();

    assertEquals(counter.get(), 1);
  }

  @Test
  void unregisterSubsystemTest() {
    Subsystem system = new Subsystem() {};

    CommandScheduler.getInstance().registerSubsystem(system);
    assertDoesNotThrow(() -> CommandScheduler.getInstance().unregisterSubsystem(system));
  }

  @Test
  void schedulerCancelAllTest() {
    AtomicInteger counter = new AtomicInteger();

    CommandScheduler.getInstance().onCommandInterrupt(command -> counter.incrementAndGet());

    Command command = new WaitCommand(10);
    Command command2 = new WaitCommand(10);

    command.schedule();
    command2.schedule();
    CommandScheduler.getInstance().cancelAll();

    assertEquals(counter.get(), 2);
  }

  @Test
  void scheduleScheduledNoOp() {
    AtomicInteger counter = new AtomicInteger();

    Command command = Commands.startEnd(counter::incrementAndGet, () -> {});

    command.schedule();
    command.schedule();

    assertEquals(counter.get(), 1);
  }
}
