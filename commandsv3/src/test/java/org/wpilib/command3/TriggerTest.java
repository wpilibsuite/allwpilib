// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;

class TriggerTest extends CommandTestBase {
  @Test
  void onTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
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
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    trigger.onFalse(command);

    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(command), "Command should be scheduled when signal starts low");

    signal.set(true);
    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(command), "Command should still be running rising falling edge");
  }

  @Test
  void whileTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    trigger.whileTrue(command);

    signal.set(true);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(command), "Command was not scheduled on rising edge");

    signal.set(false);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command), "Command should be canceled on falling edge");
  }

  @Test
  void whileFalse() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    trigger.whileFalse(command);

    m_scheduler.run();
    assertTrue(
        m_scheduler.isRunning(command), "Command should be scheduled when signal starts low");

    signal.set(true);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command), "Command should be canceled on rising edge");
  }

  @Test
  void toggleOnTrue() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
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
    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
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

    var inner =
        Command.noRequirements()
            .executing(
                co -> {
                  while (true) {
                    innerRan.set(true);
                    co.park();
                  }
                })
            .named("Inner");

    var outer =
        Command.noRequirements()
            .executing(
                co -> {
                  new Trigger(m_scheduler, innerSignal::get).onTrue(inner);
                  // If we yield, then the outer command exits and immediately cancels the
                  // on-deck inner command before it can run
                  co.park();
                })
            .named("Outer");

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

  @Test
  void scopeGoingInactiveCancelsBoundCommand() {
    var activeScope = new AtomicBoolean(true);
    BindingScope scope = activeScope::get;

    var triggerSignal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, triggerSignal::get);

    var command = Command.noRequirements().executing(Coroutine::park).named("Command");
    trigger.addBinding(scope, BindingType.RUN_WHILE_HIGH, command);

    triggerSignal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should have started when triggered");

    activeScope.set(false);
    m_scheduler.run();
    assertFalse(
        m_scheduler.isRunning(command),
        "Command should have been canceled when scope became inactive");
  }

  // The scheduler lifecycle polls triggers at the start of `run()`
  // Even though the trigger condition is set, the command exits and the trigger's scope goes
  // inactive before the next `run()` call can poll the trigger
  @Test
  void triggerFromExitingCommandDoesNotFire() {
    var condition = new AtomicBoolean(false);
    var triggeredCommandRan = new AtomicBoolean(false);

    var inner =
        Command.noRequirements()
            .executing(
                co -> {
                  triggeredCommandRan.set(true);
                  co.park();
                })
            .named("Inner");

    var awaited =
        Command.noRequirements()
            .executing(
                co -> {
                  co.yield();
                  condition.set(true);
                })
            .named("Awaited");

    var outer =
        Command.noRequirements()
            .executing(
                co -> {
                  new Trigger(m_scheduler, condition::get).onTrue(inner);
                  co.await(awaited);
                })
            .named("Outer");

    m_scheduler.schedule(outer);

    // First run: schedules `awaited`, yields
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(outer));
    assertTrue(m_scheduler.isRunning(awaited));
    assertEquals(
        List.of("Outer", "Awaited"),
        m_scheduler.getRunningCommands().stream().map(Command::name).toList());

    // Second run: `awaited` resumes, sets the condition, exits. `outer` exits its final `yield`
    // and will exit on the next run. The trigger condition has been set, but the trigger is checked
    // on the next call to `run()`
    m_scheduler.run();
    assertEquals(List.of(), m_scheduler.getRunningCommands().stream().map(Command::name).toList());
    assertTrue(condition.get(), "Condition wasn't set");
    assertFalse(triggeredCommandRan.get(), "Command was unexpectedly triggered");

    // Third run: trigger binding fires (outside a running command) and queues up `inner`.
    // However, the inner command's lifetime is bound to `outer`, and is immediately canceled before
    // it can run when the outer command exits.
    m_scheduler.run();
    assertEquals(List.of(), m_scheduler.getRunningCommands().stream().map(Command::name).toList());
    assertFalse(triggeredCommandRan.get(), "Command was unexpectedly triggered");
  }

  @Test
  void risingEdge() {
    var command = new NullCommand();

    var signal = new AtomicBoolean(false);
    var baseTrigger = new Trigger(m_scheduler, signal::get);
    var risingEdgeTrigger = baseTrigger.risingEdge();

    // Ensure bindings. Triggers aren't polled when no bindings have been added.
    baseTrigger.addBinding(BindingScope.global(), BindingType.RUN_WHILE_HIGH, command);
    risingEdgeTrigger.addBinding(BindingScope.global(), BindingType.RUN_WHILE_HIGH, command);

    assertAll(
        "Signals start null",
        () -> assertEquals(null, baseTrigger.getCachedSignal()),
        () -> assertEquals(null, baseTrigger.getPreviousSignal()),
        () -> assertEquals(null, risingEdgeTrigger.getCachedSignal()),
        () -> assertEquals(null, risingEdgeTrigger.getPreviousSignal()));

    m_scheduler.run();
    assertAll(
        "First run (base signal stays low)",
        () -> assertEquals(Trigger.Signal.LOW, baseTrigger.getCachedSignal()),
        () -> assertEquals(null, baseTrigger.getPreviousSignal()),
        () -> assertEquals(Trigger.Signal.LOW, risingEdgeTrigger.getCachedSignal()),
        () -> assertEquals(null, risingEdgeTrigger.getPreviousSignal()));

    signal.set(true);
    m_scheduler.run();
    assertAll(
        "Second run (base signal goes high)",
        () -> assertEquals(Trigger.Signal.HIGH, baseTrigger.getCachedSignal()),
        () -> assertEquals(Trigger.Signal.LOW, baseTrigger.getPreviousSignal()),
        () ->
            assertEquals(
                Trigger.Signal.HIGH,
                risingEdgeTrigger.getCachedSignal(),
                "Rising edge trigger did not go high"),
        () -> assertEquals(Trigger.Signal.LOW, risingEdgeTrigger.getPreviousSignal()));

    m_scheduler.run();
    assertAll(
        "Third run (base signal stays high)",
        () -> assertEquals(Trigger.Signal.HIGH, baseTrigger.getCachedSignal()),
        () -> assertEquals(Trigger.Signal.HIGH, baseTrigger.getPreviousSignal()),
        () ->
            assertEquals(
                Trigger.Signal.LOW,
                risingEdgeTrigger.getCachedSignal(),
                "Rising edge trigger did not go low"),
        () ->
            assertEquals(
                Trigger.Signal.HIGH,
                risingEdgeTrigger.getPreviousSignal(),
                "Rising edge trigger was not previously high"));
  }

  @Test
  void fallingEdge() {
    var command = new NullCommand();

    var signal = new AtomicBoolean(false);
    var baseTrigger = new Trigger(m_scheduler, signal::get);
    var fallingEdgeTrigger = baseTrigger.fallingEdge();

    // Ensure bindings. Triggers aren't polled when no bindings have been added.
    baseTrigger.addBinding(BindingScope.global(), BindingType.RUN_WHILE_HIGH, command);
    fallingEdgeTrigger.addBinding(BindingScope.global(), BindingType.RUN_WHILE_HIGH, command);

    assertAll(
        "Signals start null",
        () -> assertEquals(null, baseTrigger.getCachedSignal()),
        () -> assertEquals(null, baseTrigger.getPreviousSignal()),
        () -> assertEquals(null, fallingEdgeTrigger.getCachedSignal()),
        () -> assertEquals(null, fallingEdgeTrigger.getPreviousSignal()));

    m_scheduler.run();
    assertAll(
        "First run (base signal stays low)",
        () -> assertEquals(Trigger.Signal.LOW, baseTrigger.getCachedSignal()),
        () -> assertEquals(null, baseTrigger.getPreviousSignal()),
        () -> assertEquals(Trigger.Signal.LOW, fallingEdgeTrigger.getCachedSignal()),
        () -> assertEquals(null, fallingEdgeTrigger.getPreviousSignal()));

    signal.set(true);
    m_scheduler.run();
    assertAll(
        "Second run (base signal goes high)",
        () -> assertEquals(Trigger.Signal.HIGH, baseTrigger.getCachedSignal()),
        () -> assertEquals(Trigger.Signal.LOW, baseTrigger.getPreviousSignal()),
        () -> assertEquals(Trigger.Signal.LOW, fallingEdgeTrigger.getCachedSignal()),
        () -> assertEquals(Trigger.Signal.LOW, fallingEdgeTrigger.getPreviousSignal()));

    signal.set(false);
    m_scheduler.run();
    assertAll(
        "Third run (base signal goes low)",
        () -> assertEquals(Trigger.Signal.LOW, baseTrigger.getCachedSignal()),
        () -> assertEquals(Trigger.Signal.HIGH, baseTrigger.getPreviousSignal()),
        () ->
            assertEquals(
                Trigger.Signal.HIGH,
                fallingEdgeTrigger.getCachedSignal(),
                "Falling edge trigger did not go high"),
        () ->
            assertEquals(
                Trigger.Signal.LOW,
                fallingEdgeTrigger.getPreviousSignal(),
                "Falling edge trigger was not previously low"));

    m_scheduler.run();
    assertAll(
        "Fourth run (base signal stays low)",
        () -> assertEquals(Trigger.Signal.LOW, baseTrigger.getCachedSignal()),
        () -> assertEquals(Trigger.Signal.LOW, baseTrigger.getPreviousSignal()),
        () ->
            assertEquals(
                Trigger.Signal.LOW,
                fallingEdgeTrigger.getCachedSignal(),
                "Falling edge trigger did not go low"),
        () ->
            assertEquals(
                Trigger.Signal.HIGH,
                fallingEdgeTrigger.getPreviousSignal(),
                "Falling edge trigger was not previously high"));
  }
}
