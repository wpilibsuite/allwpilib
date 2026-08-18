// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.Seconds;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import java.util.function.BooleanSupplier;
import org.junit.jupiter.api.Test;
import org.wpilib.system.RobotController;

class TriggerTest extends CommandTestBase {
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
    var command = Command.noRequirements(Coroutine::park).named("Command");
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
    var command = Command.noRequirements(Coroutine::park).named("Command");
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

    var inner =
        Command.noRequirements(
                co -> {
                  while (true) {
                    innerRan.set(true);
                    co.park();
                  }
                })
            .named("Inner");

    var outer =
        Command.noRequirements(
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

    var command = Command.noRequirements(Coroutine::park).named("Command");
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

  @Test
  void bindingScopesToOpmodeIfAvailable() {
    var fetcher =
        new OpModeFetcher() {
          long m_id = 12345;

          void clear() {
            m_id = 0;
          }

          @Override
          long getOpModeId() {
            return m_id;
          }

          @Override
          String getOpModeName() {
            return "This is an opmode!";
          }
        };
    OpModeFetcher.setFetcher(fetcher);

    var triggerSignal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, triggerSignal::get);

    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.whileTrue(command);

    triggerSignal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should have started when triggered");

    fetcher.clear();
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command), "Command should have stopped when opmode exited");
  }

  // The scheduler lifecycle polls triggers at the start of `run()`
  // Even though the trigger condition is set, the command exits and the trigger's scope goes
  // inactive before the next `run()` call can poll the trigger
  @Test
  void triggerFromExitingCommandDoesNotFire() {
    var condition = new AtomicBoolean(false);
    var triggeredCommandRan = new AtomicBoolean(false);

    var inner =
        Command.noRequirements(
                co -> {
                  triggeredCommandRan.set(true);
                  co.park();
                })
            .named("Inner");

    var awaited =
        Command.noRequirements(
                co -> {
                  co.yield();
                  condition.set(true);
                })
            .named("Awaited");

    var outer =
        Command.noRequirements(
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
    var signal = new AtomicBoolean(false);
    var baseTrigger = new Trigger(m_scheduler, signal::get);
    var risingEdgeTrigger = baseTrigger.risingEdge();

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
    var signal = new AtomicBoolean(false);
    var baseTrigger = new Trigger(m_scheduler, signal::get);
    var fallingEdgeTrigger = baseTrigger.fallingEdge();

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

  @Test
  void ensureBoundBindsDependencies() {
    var a = new AtomicBoolean(false);
    var b = new AtomicBoolean(false);

    var baseA = new Trigger(m_scheduler, a::get);
    var baseB = new Trigger(m_scheduler, b::get);

    // Compose a trigger that depends on an intermediate, unbound risingEdge() trigger
    var composed = baseA.and(baseB.risingEdge());

    var command = Command.noRequirements(Coroutine::park).named("Cmd");
    // Bind only the composed trigger; ensureBound() must bind dependencies first so polling order
    // updates base triggers before evaluating the composed condition.
    composed.onTrue(command);

    // First run initializes all signals to LOW
    m_scheduler.run();
    assertFalse(
        m_scheduler.isRunning(command), "Command should not run on first initialization run");

    // Cause both conditions to be true in the same cycle: A is true, and B has a rising edge
    a.set(true);
    b.set(true);
    m_scheduler.run();

    assertTrue(
        m_scheduler.isRunning(command),
        "Top-level composed trigger did not fire when dependency rising edge occurred");
  }

  @Test
  void ensureBoundDeeplyNestedDependencies() {
    var a = new AtomicBoolean(false);
    var b = new AtomicBoolean(false);
    var c = new AtomicBoolean(false);

    var baseA = new Trigger(m_scheduler, a::get);
    var baseB = new Trigger(m_scheduler, b::get);
    var baseC = new Trigger(m_scheduler, c::get);

    // Two levels of nesting: baseA AND (baseB.risingEdge() AND baseC.risingEdge())
    final var nested = baseA.and(baseB.risingEdge().and(baseC.risingEdge()));

    // Initialize signals
    m_scheduler.run();

    // Trigger both rising edges and set A high in the same cycle
    a.set(true);
    b.set(true);
    c.set(true);
    m_scheduler.run();

    assertTrue(
        nested.getAsBoolean(),
        "Deeply nested composed trigger did not fire; dependencies may not have been bound first");
  }

  @Test
  void composedAnd() {
    var signalA = new AtomicBoolean(false);
    var signalB = new AtomicBoolean(false);
    var triggerA = new Trigger(m_scheduler, flickering(signalA));
    var triggerB = new Trigger(m_scheduler, flickering(signalB));

    var andTrigger = triggerA.and(triggerB);

    m_scheduler.run();
    assertFalse(andTrigger.getAsBoolean());

    signalA.set(true);
    m_scheduler.run();
    assertFalse(andTrigger.getAsBoolean());

    signalA.set(true);
    signalB.set(true);
    m_scheduler.run();
    assertTrue(andTrigger.getAsBoolean());

    signalA.set(false);
    m_scheduler.run();
    assertFalse(andTrigger.getAsBoolean());
  }

  @Test
  void composedAndWithSupplier() {
    var signalA = new AtomicBoolean(false);
    var signalB = new AtomicBoolean(false);
    var triggerA = new Trigger(m_scheduler, flickering(signalA));

    var andTrigger = triggerA.and(flickering(signalB));

    m_scheduler.run();
    assertFalse(andTrigger.getAsBoolean());

    signalA.set(true);
    signalB.set(true);
    m_scheduler.run();
    assertTrue(andTrigger.getAsBoolean());

    signalB.set(false);
    m_scheduler.run();
    assertFalse(andTrigger.getAsBoolean());
  }

  @Test
  void composedOr() {
    var signalA = new AtomicBoolean(false);
    var signalB = new AtomicBoolean(false);
    var triggerA = new Trigger(m_scheduler, flickering(signalA));
    var triggerB = new Trigger(m_scheduler, flickering(signalB));

    var orTrigger = triggerA.or(triggerB);

    m_scheduler.run();
    assertFalse(orTrigger.getAsBoolean());

    signalA.set(true);
    m_scheduler.run();
    assertTrue(orTrigger.getAsBoolean());

    signalA.set(false);
    m_scheduler.run();
    assertFalse(orTrigger.getAsBoolean());

    signalB.set(true);
    m_scheduler.run();
    assertTrue(orTrigger.getAsBoolean());
  }

  @Test
  void composedOrWithSupplier() {
    var signalA = new AtomicBoolean(false);
    var signalB = new AtomicBoolean(false);
    var triggerA = new Trigger(m_scheduler, flickering(signalA));

    var orTrigger = triggerA.or(flickering(signalB));

    m_scheduler.run();
    assertFalse(orTrigger.getAsBoolean());

    signalB.set(true);
    m_scheduler.run();
    assertTrue(orTrigger.getAsBoolean());
  }

  @Test
  void composedNegate() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, flickering(signal));

    var negated = trigger.negate();

    m_scheduler.run();
    assertTrue(negated.getAsBoolean());

    signal.set(true);
    m_scheduler.run();
    assertFalse(negated.getAsBoolean());
  }

  @Test
  void selfComposition() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, flickering(signal));

    var selfAnd = trigger.and(trigger);
    var selfOr = trigger.or(trigger);

    m_scheduler.run();
    assertFalse(selfAnd.getAsBoolean());
    assertFalse(selfOr.getAsBoolean());

    signal.set(true);
    m_scheduler.run();
    assertTrue(selfAnd.getAsBoolean());
    assertTrue(selfOr.getAsBoolean());
  }

  @Test
  void complexComposition() {
    var signalA = new AtomicBoolean(false);
    var signalB = new AtomicBoolean(false);
    var signalC = new AtomicBoolean(false);
    var triggerA = new Trigger(m_scheduler, flickering(signalA));
    var triggerB = new Trigger(m_scheduler, flickering(signalB));
    var triggerC = new Trigger(m_scheduler, flickering(signalC));

    // (A and B) or (not C)
    var composed = triggerA.and(triggerB).or(triggerC.negate());

    // Initially A=F, B=F, C=F. (F and F) or (not F) -> F or T -> T
    m_scheduler.run();
    assertTrue(composed.getAsBoolean());

    // A=T, B=T, C=T. (T and T) or (not T) -> T or F -> T
    signalA.set(true);
    signalB.set(true);
    signalC.set(true);
    m_scheduler.run();
    assertTrue(composed.getAsBoolean());

    // A=F, B=T, C=T. (F and T) or (not T) -> F or F -> F
    signalA.set(false);
    signalC.set(true); // Ensure C is high for next run if it flickered
    m_scheduler.run();
    assertFalse(composed.getAsBoolean());
  }

  @Test
  void triggerUnbindsWhenCommandScopeInactive() {
    var triggerSignal = new AtomicBoolean(false);
    var commandRan = new AtomicBoolean(false);
    var innerCommand = Command.noRequirements(_ -> commandRan.set(true)).named("Inner");

    var outerCommand =
        Command.noRequirements(
                co -> {
                  var trigger = new Trigger(m_scheduler, triggerSignal::get);
                  trigger.onTrue(innerCommand);
                  co.park();
                })
            .named("Outer");

    m_scheduler.schedule(outerCommand);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(outerCommand));

    triggerSignal.set(true);
    m_scheduler.run();
    assertTrue(commandRan.get());

    // Cancel outer command, trigger should now be out of scope
    m_scheduler.cancel(outerCommand);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(outerCommand));

    // The trigger should have unbound itself during the last run() call.
  }

  @Test
  void multiPress() {
    var currentTimeMicros = new AtomicLong(1000000); // Start at 1s
    RobotController.setTimeSource(currentTimeMicros::get);

    var signal = new AtomicBoolean(false);
    var baseTrigger = new Trigger(m_scheduler, signal::get);
    var multiPressTrigger = baseTrigger.multiPress(3, Seconds.of(1));

    m_scheduler.run();
    assertFalse(multiPressTrigger.getAsBoolean(), "Should not fire initially");

    // First press at 1.1s
    currentTimeMicros.set(1100000);
    signal.set(true);
    m_scheduler.run();
    assertFalse(multiPressTrigger.getAsBoolean(), "Should not fire after 1 press");

    signal.set(false);
    m_scheduler.run();

    // Second press at 1.2s
    currentTimeMicros.set(1200000);
    signal.set(true);
    m_scheduler.run();
    assertFalse(multiPressTrigger.getAsBoolean(), "Should not fire after 2 presses");

    signal.set(false);
    m_scheduler.run();

    // Third press at 1.3s
    currentTimeMicros.set(1300000);
    signal.set(true);
    m_scheduler.run();
    assertTrue(multiPressTrigger.getAsBoolean(), "Should fire after 3 presses");

    signal.set(false);
    m_scheduler.run();

    // Fourth press at 2.0s (First press at 1.1s should be NOT yet expired, so 1.1s, 1.2s, 1.3s,
    // 2.0s ->
    // 4 presses)
    currentTimeMicros.set(2000000);
    signal.set(true);
    m_scheduler.run();
    assertTrue(
        multiPressTrigger.getAsBoolean(),
        "Should still fire as there are 4 presses within last 1s");

    signal.set(false);
    m_scheduler.run();

    // Wait until 2.2s. Press at 1.1s is expired (exactly 1.1s elapsed).
    // Remaining: 1.2s, 1.3s, 2.0s -> 3 presses.
    currentTimeMicros.set(2200000);
    m_scheduler.run();
    assertTrue(
        multiPressTrigger.getAsBoolean(),
        "Should still fire as there are 3 presses within last 1s");

    // Wait until 2.4s. Presses at 1.2s and 1.3s are definitely expired. Only 2.0s remains.
    currentTimeMicros.set(2400000);
    m_scheduler.run();
    assertFalse(multiPressTrigger.getAsBoolean(), "Should not fire after presses expire");
  }

  @Test
  void multiPressZeroDuration() {
    var trigger = new Trigger(m_scheduler, () -> true);
    var zeroDuration = trigger.multiPress(1, Seconds.of(0));
    m_scheduler.run();
    assertFalse(zeroDuration.getAsBoolean(), "Zero duration multiPress should always be false");
  }

  @Test
  void multiPressNegativeDuration() {
    var trigger = new Trigger(m_scheduler, () -> true);
    var negativeDuration = trigger.multiPress(1, Seconds.of(-1));
    m_scheduler.run();
    assertFalse(
        negativeDuration.getAsBoolean(), "Negative duration multiPress should always be false");
  }

  @Test
  void multiPressZeroPressCount() {
    var trigger = new Trigger(m_scheduler, () -> true);
    var zeroPressCount = trigger.multiPress(0, Seconds.of(1));
    m_scheduler.run();
    assertTrue(zeroPressCount.getAsBoolean(), "Zero press count multiPress should always be true");
  }

  @Test
  void multiPressNegativePressCount() {
    var trigger = new Trigger(m_scheduler, () -> true);
    var negativePressCount = trigger.multiPress(-1, Seconds.of(1));
    m_scheduler.run();
    assertTrue(
        negativePressCount.getAsBoolean(), "Negative press count multiPress should always be true");
  }

  @Test
  void retryWhileTrueLongRunningCanceled() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.retryWhileTrue(command);

    signal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should be running when signal is true");

    signal.set(false);
    m_scheduler.run();
    assertFalse(
        m_scheduler.isRunning(command), "Command should be canceled when signal goes false");
  }

  @Test
  void retryWhileTrueLongRunningRestarted() {
    var signal = new AtomicBoolean(true);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.retryWhileTrue(command);

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should be running initially");

    m_scheduler.cancel(command);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should be restarted in next cycle");
  }

  @Test
  void retryWhileTrueOneShotRestarted() {
    var signal = new AtomicBoolean(true);
    var trigger = new Trigger(m_scheduler, signal::get);
    var counter = new AtomicLong(0);
    var oneshot = Command.noRequirements(_ -> counter.incrementAndGet()).named("One Shot");
    trigger.retryWhileTrue(oneshot);

    m_scheduler.run();
    assertEquals(1, counter.get(), "Command should have run once");

    m_scheduler.run();
    assertEquals(2, counter.get(), "Command should have run twice");
  }

  @Test
  void retryWhileFalseLongRunningCanceled() {
    var signal = new AtomicBoolean(true);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.retryWhileFalse(command);

    signal.set(false);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should be running when signal is false");

    signal.set(true);
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(command), "Command should be canceled when signal goes true");
  }

  @Test
  void retryWhileFalseLongRunningRestarted() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var command = Command.noRequirements(Coroutine::park).named("Command");
    trigger.retryWhileFalse(command);

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should be running initially");

    m_scheduler.cancel(command);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(command), "Command should be restarted in next cycle");
  }

  @Test
  void retryWhileFalseOneShotRestarted() {
    var signal = new AtomicBoolean(false);
    var trigger = new Trigger(m_scheduler, signal::get);
    var counter = new AtomicLong(0);
    var oneshot = Command.noRequirements(_ -> counter.incrementAndGet()).named("One Shot");
    trigger.retryWhileFalse(oneshot);

    m_scheduler.run();
    assertEquals(1, counter.get(), "Command should have run once");

    m_scheduler.run();
    assertEquals(2, counter.get(), "Command should have run twice");
  }

  private BooleanSupplier flickering(AtomicBoolean signal) {
    return () -> {
      boolean val = signal.get();
      if (val) {
        signal.set(false);
      }
      return val;
    };
  }
}
