// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class TriggerTest {
  private Scheduler m_scheduler;

  @BeforeEach
  void setup() {
    m_scheduler = new Scheduler();
  }

  @Test
  void onTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.onTrue(command);

    signal.set(true);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(command), "Command was not scheduled on rising edge");

    signal.set(false);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should still be running on falling edge");
  }

  @Test
  void onFalse() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.onFalse(command);

    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(command),
        "Command should be scheduled when signal starts low");

    signal.set(true);
    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(command),
        "Command should still be running rising falling edge");
  }

  @Test
  void whileTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.whileTrue(command);

    signal.set(true);
    m_scheduler.run();

    assertTrue(
        m_scheduler.isRunning(command),
        "Command was not scheduled on rising edge");

    signal.set(false);
    m_scheduler.run();
    assertFalse(
        m_scheduler.isRunning(command),
        "Command should be cancelled on falling edge");
  }

  @Test
  void whileFalse() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.whileFalse(command);

    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(command),
        "Command should be scheduled when signal starts low");

    signal.set(true);
    m_scheduler.run();
    assertFalse(
        m_scheduler.isRunning(command),
        "Command should be cancelled on rising edge");
  }

  @Test
  void toggleOnTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.toggleOnTrue(command);

    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command));

    signal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));

    signal.set(false);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));

    signal.set(true);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void toggleOnFalse() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.toggleOnFalse(command);

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));

    signal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command));

    signal.set(false);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void commandScoping() {
    var innerRan = new AtomicBoolean(false);
    var innerSignal = new AtomicBoolean(false);

    var inner = Command.noRequirements(co -> {
      while (true) {
        innerRan.set(true);
        co.park();
      }
    }).named("Inner");

    var outer = Command.noRequirements(co -> {
      new Trigger(m_scheduler, innerSignal::get).onTrue(inner);
      co.yield();
    }).named("Outer");

    m_scheduler.schedule(outer);
    m_scheduler.run();
    assertFalse(innerRan.get(), "The bound command should not run before the signal is set");

    innerSignal.set(true);
    m_scheduler.run();
    assertTrue(innerRan.get(), "The nested trigger should have run the bound command");

    innerRan.set(false);
    m_scheduler.run();
    assertFalse(innerRan.get(), "Trigger should not have fired again");
  }
}
