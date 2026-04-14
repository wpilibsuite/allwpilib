// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.command3.SchedulerEvent.Canceled;
import static org.wpilib.command3.SchedulerEvent.Mounted;
import static org.wpilib.command3.SchedulerEvent.Scheduled;
import static org.wpilib.command3.SchedulerEvent.Yielded;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;
import org.wpilib.annotation.PostConstructionInitializer;

@SuppressWarnings("PMD.CompareObjectsWithEquals")
class StateMachineTest extends CommandTestBase {
  @Test
  @SuppressWarnings(PostConstructionInitializer.SUPPRESSION_KEY)
  void errorsWithoutInitialState() {
    Mechanism mech = new Mechanism("Mechanism", m_scheduler);
    Command command1 = mech.run(Coroutine::park).named("Command 1");
    Command command2 = mech.run(Coroutine::park).named("Command 2");

    StateMachine stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    // stateMachine.setInitialState(state1); // Oops, someone forgot to set the initial state!
    state1.switchTo(state2).whenComplete();

    m_scheduler.schedule(stateMachine);

    // Don't worry, it'll be caught at runtime.
    // It would actually be caught at compile time, but we disabled the compiler check for this test
    var exception = assertThrows(IllegalStateException.class, () -> m_scheduler.run());
    assertEquals(
        "State Machine does not have an initial state. Use .setInitialState() to provide one.",
        exception.getMessage());
    assertFalse(m_scheduler.isRunning(stateMachine), "State machine should not be running");
  }

  @Test
  void initialStateCanBeOverridden() {
    Mechanism mech = new Mechanism("Mechanism", m_scheduler);
    Command command1 = mech.run(Coroutine::park).named("Command 1");
    Command command2 = mech.run(Coroutine::park).named("Command 2");

    StateMachine stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    stateMachine.setInitialState(state1);
    stateMachine.setInitialState(state2);
    state2.switchTo(state1).whenComplete();

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertTrue(m_scheduler.isRunning(command2), "Command 2 should be running as the initial state");
    assertFalse(m_scheduler.isRunning(command1), "Command 1 should not be running");
  }

  @Test
  void transitions() {
    AtomicBoolean signalA = new AtomicBoolean(false);
    AtomicBoolean signalB = new AtomicBoolean(false);

    Mechanism mech = new Mechanism("Mechanism", m_scheduler);
    var command1 = mech.run(Coroutine::park).named("Command 1");
    var command2 = mech.run(Coroutine::park).named("Command 2");
    var command3 = mech.run(Coroutine::park).named("Command 3");

    StateMachine stateMachine = new StateMachine("State Machine");

    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);

    stateMachine.setInitialState(state1);

    state1.switchTo(state2).when(signalA::get);
    state2.switchTo(state3).when(signalB::get);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertAll(
        () -> assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running"),
        () -> assertTrue(m_scheduler.isRunning(command1), "Command 1 should be running"),
        () -> assertFalse(m_scheduler.isRunning(command2), "Command 2 should not be running"),
        () -> assertFalse(m_scheduler.isRunning(command3), "Command 3 should not be running"));

    signalA.set(true);
    m_scheduler.run();
    assertAll(
        () -> assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running"),
        () -> assertFalse(m_scheduler.isRunning(command1), "Command 1 should not be running"),
        () -> assertTrue(m_scheduler.isRunning(command2), "Command 2 should be running"),
        () -> assertFalse(m_scheduler.isRunning(command3), "Command 3 should not be running"));

    signalB.set(true);
    m_scheduler.run();
    assertAll(
        () -> assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running"),
        () -> assertFalse(m_scheduler.isRunning(command1), "Command 1 should not be running"),
        () -> assertFalse(m_scheduler.isRunning(command2), "Command 2 should not be running"),
        () -> assertTrue(m_scheduler.isRunning(command3), "Command 3 should be running"));
  }

  @Test
  void transitionsIfConditionIsAlreadyTrueWhenEntered() {
    var command1 = Command.noRequirements().executing(Coroutine::park).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");

    var signal = new AtomicBoolean(false);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    stateMachine.setInitialState(state1);
    state1.switchTo(state2).when(signal::get);

    signal.set(true);
    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertFalse(m_scheduler.isRunning(command1), "Command 1 should not be running");
    assertTrue(m_scheduler.isRunning(command2), "State 1 should have transitioned to State 2");
  }

  @Test
  void commandExits() {
    AtomicBoolean signal = new AtomicBoolean(false);

    var command1 =
        Command.noRequirements().executing(co -> co.waitUntil(signal::get)).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);

    stateMachine.setInitialState(state1);

    state1.switchTo(state2).whenComplete();
    state2.exitStateMachine().whenComplete();

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertTrue(m_scheduler.isRunning(command1), "Command 1 should be running");

    signal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertFalse(m_scheduler.isRunning(command1), "Command 1 should have ended");
    assertTrue(m_scheduler.isRunning(command2), "Command 2 should have started");
  }

  @Test
  void stateTransitionsToSelf() {
    AtomicBoolean signal = new AtomicBoolean(false);
    AtomicInteger initCount = new AtomicInteger(0);

    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  initCount.incrementAndGet();
                  co.park();
                })
            .named("Command");
    var stateMachine = new StateMachine("State Machine");
    var state = stateMachine.addState(command);
    stateMachine.setInitialState(state);
    state.switchTo(state).when(signal::get);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertEquals(1, initCount.get(), "Command should be initialized once");

    signal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should still be running");
    assertEquals(2, initCount.get(), "Command should have reinitialized");

    assertEquals(14, m_events.size());
    assertAll(
        // First run
        () -> assertTrue(m_events.get(0) instanceof Scheduled s && s.command() == stateMachine),
        () -> assertTrue(m_events.get(1) instanceof Mounted m && m.command() == stateMachine),
        () -> assertTrue(m_events.get(2) instanceof Scheduled s && s.command() == command),
        () -> assertTrue(m_events.get(3) instanceof Mounted m && m.command() == command),
        () -> assertTrue(m_events.get(4) instanceof Yielded y && y.command() == command),
        () -> assertTrue(m_events.get(5) instanceof Yielded y && y.command() == stateMachine),
        () -> assertTrue(m_events.get(6) instanceof Mounted m && m.command() == command),
        () -> assertTrue(m_events.get(7) instanceof Yielded y && y.command() == command),
        // Second run
        () -> assertTrue(m_events.get(8) instanceof Mounted m && m.command() == stateMachine),
        () -> assertTrue(m_events.get(9) instanceof Canceled c && c.command() == command),
        () -> assertTrue(m_events.get(10) instanceof Scheduled s && s.command() == command),
        () -> assertTrue(m_events.get(11) instanceof Mounted m && m.command() == command),
        () -> assertTrue(m_events.get(12) instanceof Yielded y && y.command() == command),
        () -> assertTrue(m_events.get(13) instanceof Yielded y && y.command() == stateMachine));
  }

  @Test
  void oneshotCommandTransitionsToSelfOnComplete() {
    AtomicInteger count = new AtomicInteger(0);
    var command = Command.noRequirements().executing(c -> count.incrementAndGet()).named("Command");
    var stateMachine = new StateMachine("State Machine");
    var state = stateMachine.addState(command);
    stateMachine.setInitialState(state);
    state.switchTo(state).whenComplete();

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertEquals(1, count.get(), "Command should have run once");
  }

  @Test
  void onlyFirstExplicitTransitionFires() {
    var signal = new AtomicBoolean(false);

    var command1 = Command.noRequirements().executing(Coroutine::park).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");
    var command3 = Command.noRequirements().executing(Coroutine::park).named("Command 3");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);

    stateMachine.setInitialState(state1);
    state1.switchTo(state2).when(signal::get);
    state1.switchTo(state3).when(signal::get);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(List.of(stateMachine, command1), m_scheduler.getRunningCommands());

    signal.set(true);
    m_scheduler.run();
    assertEquals(List.of(stateMachine, command2), m_scheduler.getRunningCommands());
  }

  @Test
  void onlyLastWhenCompleteTransitionFires() {
    var command1 = Command.noRequirements().executing(Coroutine::yield).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::yield).named("Command 2");
    var command3 = Command.noRequirements().executing(Coroutine::yield).named("Command 3");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);

    stateMachine.setInitialState(state1);
    state1.switchTo(state2).whenComplete();
    state1.switchTo(state3).whenComplete(); // overrides the previous transition

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(List.of(stateMachine, command1), m_scheduler.getRunningCommands());

    m_scheduler.run();
    assertEquals(List.of(stateMachine, command3), m_scheduler.getRunningCommands());
  }

  @Test
  void whenCompleteAndTakesPriorityOverWhenCompleteIfCalledLast() {
    var signal = new AtomicBoolean(false);

    var command1 = Command.noRequirements().executing(Coroutine::yield).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::yield).named("Command 2");
    var command3 = Command.noRequirements().executing(Coroutine::yield).named("Command 3");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);

    stateMachine.setInitialState(state1);
    state1.switchTo(state2).whenComplete();
    state1.switchTo(state3).whenCompleteAnd(signal::get);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(List.of(stateMachine, command1), m_scheduler.getRunningCommands());

    signal.set(true);
    m_scheduler.run();
    assertEquals(
        List.of(stateMachine, command3), // would be command2 if `whenComplete` took precedence
        m_scheduler.getRunningCommands());
  }

  @Test
  void whenCompleteAndTakesPriorityOverWhenCompleteIfCalleFirst() {
    var signal = new AtomicBoolean(false);

    var command1 = Command.noRequirements().executing(Coroutine::yield).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::yield).named("Command 2");
    var command3 = Command.noRequirements().executing(Coroutine::yield).named("Command 3");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);

    stateMachine.setInitialState(state1);
    state1.switchTo(state3).whenCompleteAnd(signal::get);
    state1.switchTo(state2).whenComplete();

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(List.of(stateMachine, command1), m_scheduler.getRunningCommands());

    signal.set(true);
    m_scheduler.run();
    assertEquals(
        List.of(stateMachine, command3), // would be command3 if `whenCompleteAnd` took precedence
        m_scheduler.getRunningCommands());
  }

  @Test
  void composingComplete() {
    AtomicBoolean signal = new AtomicBoolean(false);
    var command1 = Command.noRequirements().executing(Coroutine::yield).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);

    stateMachine.setInitialState(state1);
    state1.exitStateMachine().whenComplete();
    state1.switchTo(state2).whenCompleteAnd(signal::get);

    // First run, signal is low - state machine exits on state completion
    {
      m_scheduler.schedule(stateMachine);
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
      assertTrue(m_scheduler.isRunning(command1), "Command should be running");

      m_scheduler.run();
      assertFalse(m_scheduler.isRunning(stateMachine), "State machine should have exited");
    }

    // Second run, signal goes high - state machine switches to state2 instead of exiting
    {
      m_scheduler.schedule(stateMachine);
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
      assertTrue(m_scheduler.isRunning(command1), "Command should be running");

      signal.set(true);
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
      assertFalse(m_scheduler.isRunning(command1), "Command should have ended");
      assertTrue(m_scheduler.isRunning(command2), "Command 2 should have started");
    }
  }

  @Test
  void switchFromAny() {
    var command1 = Command.noRequirements().executing(Coroutine::yield).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");
    var command3 = Command.noRequirements().executing(Coroutine::park).named("Command 3");

    AtomicBoolean signal = new AtomicBoolean(false);

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);

    stateMachine.setInitialState(state1);
    stateMachine.switchFromAny(state1, state2).to(state3).when(signal::get);
    state1.switchTo(state2).whenComplete();

    // transition from 1 -> 3
    {
      m_scheduler.schedule(stateMachine);
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
      assertTrue(m_scheduler.isRunning(command1), "Command 1 should be running");

      signal.set(true);
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
      assertFalse(m_scheduler.isRunning(command1), "Command 1 should have ended");
      assertTrue(m_scheduler.isRunning(command3), "Command 3 should have started");
    }

    m_scheduler.cancel(stateMachine);
    signal.set(false);

    // transition from 2 -> 3
    {
      m_scheduler.schedule(stateMachine);
      m_scheduler.run(); // yield 1
      assertEquals(
          List.of("State Machine", "Command 1"),
          m_scheduler.getRunningCommands().stream().map(Command::name).toList());

      m_scheduler.run(); // transition 1 -> 2
      assertEquals(
          List.of("State Machine", "Command 2"),
          m_scheduler.getRunningCommands().stream().map(Command::name).toList());

      signal.set(true);
      m_scheduler.run(); // transition 2 -> 3
      assertEquals(
          List.of("State Machine", "Command 3"),
          m_scheduler.getRunningCommands().stream().map(Command::name).toList());
    }
  }

  @Test
  void switchToSupplierWhenComplete() {
    AtomicInteger count = new AtomicInteger(0);

    var command1 =
        Command.noRequirements()
            .executing(
                co -> {
                  count.incrementAndGet();
                  co.yield();
                })
            .named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");
    var command3 = Command.noRequirements().executing(Coroutine::park).named("Command 3");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);
    stateMachine.setInitialState(state1);
    state1
        .switchTo(
            () -> {
              if (count.get() == 1) {
                return state2;
              } else {
                return state3;
              }
            })
        .whenComplete();

    m_scheduler.schedule(stateMachine);
    m_scheduler.run(); // command 1 increments the count and then yields
    assertEquals(List.of(stateMachine, command1), m_scheduler.getRunningCommands());

    // command 1 completes, state machine moves to the next state
    // if the supplier is checked at configuration time, the count would be 0 and return state3
    // if the supplier is checked at runtime, the count would be 1 and return state2
    m_scheduler.run();
    assertEquals(List.of(stateMachine, command2), m_scheduler.getRunningCommands());
  }

  @Test
  void switchToSupplierWithCondition() {
    AtomicInteger count = new AtomicInteger(0);
    var command1 =
        Command.noRequirements()
            .executing(
                co -> {
                  while (true) {
                    // Increment after yielding. Otherwise, the condition is checked and the state
                    // machine immediately switches to the next state all within the first cycle;
                    // the running command1 is never observed.
                    co.yield();
                    count.incrementAndGet();
                  }
                })
            .named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");
    var command3 = Command.noRequirements().executing(Coroutine::park).named("Command 3");

    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    var state3 = stateMachine.addState(command3);
    stateMachine.setInitialState(state1);
    state1
        .switchTo(
            () -> {
              if (count.get() == 1) {
                return state2;
              } else {
                return state3;
              }
            })
        .when(() -> count.get() == 1);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(List.of(stateMachine, command1), m_scheduler.getRunningCommands());

    m_scheduler.run();
    assertEquals(List.of(stateMachine, command2), m_scheduler.getRunningCommands());
  }

  @Test
  void runsOnEnterForInitialState() {
    var command1 = Command.noRequirements().executing(Coroutine::park).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");

    AtomicInteger enterCount = new AtomicInteger(0);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    stateMachine.setInitialState(state1);
    state1.onEnter(enterCount::incrementAndGet);
    state1.switchTo(state2).whenComplete();

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(1, enterCount.get(), "onEnter should have been called once");
  }

  @Test
  void runsOnExitOnTransition() {
    var command1 = Command.noRequirements().executing(Coroutine::park).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");

    AtomicInteger exitCount = new AtomicInteger(0);
    AtomicBoolean signal = new AtomicBoolean(false);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    stateMachine.setInitialState(state1);
    state1.onExit(exitCount::incrementAndGet);
    state1.switchTo(state2).when(signal::get);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(0, exitCount.get(), "onExit should not have been called");

    signal.set(true);
    m_scheduler.run();
    assertEquals(1, exitCount.get(), "onExit should have been called");
  }

  @Test
  void runsOnExitWhenComplete() {
    var command1 = Command.noRequirements().executing(co -> {}).named("Command 1");

    AtomicInteger exitCount = new AtomicInteger(0);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    stateMachine.setInitialState(state1);
    state1.onExit(exitCount::incrementAndGet);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(1, exitCount.get(), "onExit should have been called");
    assertFalse(m_scheduler.isRunning(command1), "State should have exited");
  }

  @Test
  void onExitCanSchedule() {
    var mech = new Mechanism("Mechanism", m_scheduler);
    var mainMechCommand = mech.run(Coroutine::park).named("Main Mech Command");
    var backgroundMechCommand = mech.run(Coroutine::park).named("Background Mech Command");
    var nextStateCommand = Command.noRequirements().executing(Coroutine::park).named("Next");

    AtomicBoolean signal = new AtomicBoolean(false);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(mainMechCommand);
    var state2 = stateMachine.addState(nextStateCommand);
    stateMachine.setInitialState(state1);
    state1.switchTo(state2).when(signal::get);
    state1.onExit(() -> m_scheduler.schedule(backgroundMechCommand));

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertTrue(m_scheduler.isRunning(mainMechCommand), "Main Mechanism should be running");

    signal.set(true);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(stateMachine), "State machine should be running");
    assertFalse(m_scheduler.isRunning(mainMechCommand), "Main Mechanism should have ended");
    assertTrue(
        m_scheduler.isRunning(backgroundMechCommand), "Background Mechanism should have started");
    assertTrue(m_scheduler.isRunning(nextStateCommand), "Next State should have started");
  }

  @Test
  void runsOnEnterCallbacksInInsertionOrder() {
    var command1 = Command.noRequirements().executing(co -> {}).named("Command 1");

    List<String> callbackInfo = new ArrayList<>();
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    stateMachine.setInitialState(state1);
    state1.onEnter(() -> callbackInfo.add("onEnter 1"));
    state1.onEnter(() -> callbackInfo.add("onEnter 2"));

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(
        List.of("onEnter 1", "onEnter 2"), callbackInfo, "onEnter callbacks did not run correctly");
  }

  @Test
  void runsOnExitCallbacksInInsertionOrder() {
    // Make the command immediately exit
    var command1 = Command.noRequirements().executing(co -> {}).named("Command 1");

    List<String> callbackInfo = new ArrayList<>();
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    stateMachine.setInitialState(state1);
    state1.onExit(() -> callbackInfo.add("onExit 1"));
    state1.onExit(() -> callbackInfo.add("onExit 2"));

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertEquals(
        List.of("onExit 1", "onExit 2"), callbackInfo, "onExit callbacks did not run correctly");
  }

  @Test
  void onEnterSeesNewCommand() {
    var command1 = Command.noRequirements().executing(Coroutine::park).named("Command 1");

    AtomicBoolean sawCommand1OnEnter = new AtomicBoolean(false);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    stateMachine.setInitialState(state1);
    state1.onEnter(() -> sawCommand1OnEnter.set(m_scheduler.isRunning(command1)));

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();
    assertTrue(sawCommand1OnEnter.get(), "onEnter should have seen the command running");
  }

  @Test
  void onExitWithTransitionSeesExitedCommand() {
    var command1 = Command.noRequirements().executing(Coroutine::park).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");

    AtomicBoolean sawCommand1OnExit = new AtomicBoolean(false);
    AtomicBoolean signal = new AtomicBoolean(false);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    stateMachine.setInitialState(state1);
    state1.onExit(() -> sawCommand1OnExit.set(m_scheduler.isRunning(command1)));
    state1.switchTo(state2).when(signal::get);

    m_scheduler.schedule(stateMachine);
    m_scheduler.run();

    signal.set(true);
    m_scheduler.run();
    assertTrue(sawCommand1OnExit.get(), "onExit should have seen the exiting command");
  }

  // Because completion is defined as the command finishing on its own, callbacks will never
  // be able to see the command running in the scheduler because they're invoked _after_ the
  // command has finished.
  @Test
  void onExitWithCompleteCannotSeeExitedCommand() {
    var command1 = Command.noRequirements().executing(Coroutine::yield).named("Command 1");
    var command2 = Command.noRequirements().executing(Coroutine::park).named("Command 2");

    AtomicBoolean onExitCalled = new AtomicBoolean(false);
    AtomicBoolean sawCommand1OnExit = new AtomicBoolean(false);
    var stateMachine = new StateMachine("State Machine");
    var state1 = stateMachine.addState(command1);
    var state2 = stateMachine.addState(command2);
    stateMachine.setInitialState(state1);
    state1.onExit(
        () -> {
          onExitCalled.set(true);
          sawCommand1OnExit.set(m_scheduler.isRunning(command1));
        });
    state1.switchTo(state2).whenComplete();

    m_scheduler.schedule(stateMachine);
    m_scheduler.run(); // command yields...
    assertFalse(onExitCalled.get(), "onExit should not have been called yet");

    m_scheduler.run(); // ...then exits here
    assertTrue(onExitCalled.get(), "onExit should have been called");
    assertFalse(sawCommand1OnExit.get(), "exiting command should be invisible");
  }

  @Test
  void ledStateMachine() {
    var leds =
        new Mechanism("LEDs", m_scheduler) {
          Command idleAnimation() {
            return run(Coroutine::park).withPriority(-1).named("Default Animation");
          }

          Command infoAnimation() {
            return run(Coroutine::yield).withPriority(0).named("Info");
          }

          Command warningAnimation() {
            return run(Coroutine::yield).withPriority(1).named("Warning");
          }
        };

    Trigger normalPriorityEvent = new Trigger(() -> true);
    Trigger highPriorityEvent = new Trigger(() -> true);

    StateMachine stateMachine = new StateMachine("State Machine");

    var idleState = stateMachine.addState(leds.idleAnimation());
    var infoState = stateMachine.addState(leds.infoAnimation());
    var warningState = stateMachine.addState(leds.warningAnimation());

    stateMachine.setInitialState(idleState);

    idleState.switchTo(infoState).when(normalPriorityEvent.and(highPriorityEvent.negate()));
    idleState.switchTo(warningState).when(highPriorityEvent);

    warningState.switchTo(infoState).whenCompleteAnd(normalPriorityEvent);
    infoState.switchTo(warningState).whenCompleteAnd(highPriorityEvent);

    stateMachine.switchFromAny().to(warningState).when(highPriorityEvent);
    stateMachine.switchFromAny().to(idleState).whenComplete();
  }
}
