// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.button;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.wpilib.commands3.Command;
import org.wpilib.commands3.Coroutine;
import org.wpilib.commands3.Scheduler;
import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class TriggerTest {
  private Scheduler scheduler;

  @BeforeEach
  void setup() {
    scheduler = new Scheduler();
  }

  @Test
  void onTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.onTrue(command);

    signal.set(true);
    scheduler.run();

    assertTrue(scheduler.isRunning(command), "Command was not scheduled on rising edge");

    signal.set(false);
    scheduler.run();
    assertTrue(scheduler.isRunning(command), "Command should still be running on falling edge");
  }

  @Test
  void onFalse() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.onFalse(command);

    scheduler.run();
    assertTrue(scheduler.isRunning(command), "Command should be scheduled when signal starts low");

    signal.set(true);
    scheduler.run();
    assertTrue(scheduler.isRunning(command), "Command should still be running rising falling edge");
  }

  @Test
  void whileTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.whileTrue(command);

    signal.set(true);
    scheduler.run();

    assertTrue(scheduler.isRunning(command), "Command was not scheduled on rising edge");

    signal.set(false);
    scheduler.run();
    assertFalse(scheduler.isRunning(command), "Command should be cancelled on falling edge");
  }

  @Test
  void whileFalse() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.whileFalse(command);

    scheduler.run();
    assertTrue(scheduler.isRunning(command), "Command should be scheduled when signal starts low");

    signal.set(true);
    scheduler.run();
    assertFalse(scheduler.isRunning(command), "Command should be cancelled on rising edge");
  }

  @Test
  void toggleOnTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.toggleOnTrue(command);

    scheduler.run();
    assertFalse(scheduler.isRunning(command));

    signal.set(true);
    scheduler.run();
    assertTrue(scheduler.isRunning(command));

    signal.set(false);
    scheduler.run();
    assertTrue(scheduler.isRunning(command));

    signal.set(true);
    scheduler.run();
    assertFalse(scheduler.isRunning(command));
  }

  @Test
  void toggleOnFalse() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.toggleOnFalse(command);

    scheduler.run();
    assertTrue(scheduler.isRunning(command));

    signal.set(true);
    scheduler.run();
    assertTrue(scheduler.isRunning(command));

    signal.set(false);
    scheduler.run();
    assertFalse(scheduler.isRunning(command));
  }
}
