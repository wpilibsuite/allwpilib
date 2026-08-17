// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.units.Units.Seconds;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;
import org.wpilib.system.RobotController;

class TriggerRescopeDependencyTest extends TriggerRescopeTestBase {
  @Test
  void bindingsOnScopedCachedTriggerRunInLaterScopes() {
    m_opModeId = 1;
    m_opModeName = "mode 1";

    // Create a trigger in opmode 1
    final var signal = new AtomicBoolean(true);
    final var trigger = new Trigger(m_scheduler, m_scheduler.getDefaultEventLoop(), signal::get);

    m_scheduler.run();

    m_opModeId = 2;
    m_opModeName = "mode 2";

    // Ensure the trigger is unbound from the scheduler
    m_scheduler.run();
    assertFalse(trigger.isBound(), "Trigger should have been unbound when mode 1 exited");

    // Trigger value should be frozen since no bindings are active
    signal.set(false);
    m_scheduler.run();
    assertTrue(trigger.getAsBoolean(), "Trigger value should be frozen");

    // Then add a binding scoped to opmode 2
    var ran = new AtomicBoolean(false);
    var command = Command.noRequirements(_ -> ran.set(true)).named("Command");
    trigger.retryWhileTrue(command);

    // New binding should still run
    signal.set(true);
    m_scheduler.run();
    assertTrue(ran.get(), "Bound command did not run");
  }

  @ParameterizedTest(name = "unary variant {0}")
  @MethodSource("unaryRescopeVariants")
  void ensureBoundRescopesUnaryDependenciesAfterDescope(UnaryVariant variant) {
    var signal = new AtomicBoolean(variant.initialSignal());
    var currentTimeMicros = new AtomicLong(1_000_000);
    RobotController.setTimeSource(currentTimeMicros::get);

    var baseTriggerRef = new AtomicReference<Trigger>();
    var derivedTriggerRef = new AtomicReference<Trigger>();

    final var exitFirstScope =
        enterScope(
            ScopeType.COMMAND,
            () -> {
              var baseTrigger = new Trigger(m_scheduler, signal::get);
              baseTriggerRef.set(baseTrigger);
              derivedTriggerRef.set(variant.derive(baseTrigger));
            });

    final var baseTrigger = baseTriggerRef.get();
    final var derivedTrigger = derivedTriggerRef.get();

    m_scheduler.run();
    exitFirstScope.run();
    m_scheduler.run();

    assertFalse(baseTrigger.isBound(), "Base trigger should be unbound after descoping");
    assertFalse(derivedTrigger.isBound(), "Derived trigger should be unbound after descoping");

    var runs = new AtomicLong();
    var command =
        Command.noRequirements(_ -> runs.incrementAndGet())
            .named("Unary re-scope command " + variant);
    final var exitSecondScope = enterScope(ScopeType.OPMODE, () -> derivedTrigger.onTrue(command));

    assertTrue(
        baseTrigger.isBound(), "Base dependency should be rebound when derived trigger binds");
    assertTrue(derivedTrigger.isBound(), "Derived trigger should be rebound in new scope");

    m_scheduler.run();
    assertEquals(0, runs.get(), "Unary trigger should not fire before stimulus");

    variant.stimulate(this, signal, currentTimeMicros);

    assertEquals(1, runs.get(), "Re-scoped unary trigger did not fire for " + variant);

    exitSecondScope.run();
  }

  @ParameterizedTest(name = "binary variant {0} with {1}")
  @MethodSource("binaryRescopeVariants")
  void ensureBoundRescopesBinaryDependenciesAfterDescope(
      BinaryComposition composition, DependencyDescopeCase descopeCase) {
    var signalA = new AtomicBoolean(false);
    var signalB = new AtomicBoolean(false);
    var triggerARef = new AtomicReference<Trigger>();
    var triggerBRef = new AtomicReference<Trigger>();
    var composedRef = new AtomicReference<Trigger>();

    Runnable exitFirstScope =
        switch (descopeCase) {
          case A_DESCOPED_B_IN_SCOPE -> {
            triggerBRef.set(new Trigger(m_scheduler, signalB::get));
            yield enterScope(
                ScopeType.COMMAND,
                () -> {
                  var triggerA = new Trigger(m_scheduler, signalA::get);
                  triggerARef.set(triggerA);
                  composedRef.set(composition.compose(triggerA, triggerBRef.get()));
                });
          }
          case A_IN_SCOPE_B_DESCOPED -> {
            triggerARef.set(new Trigger(m_scheduler, signalA::get));
            yield enterScope(
                ScopeType.COMMAND,
                () -> {
                  var triggerB = new Trigger(m_scheduler, signalB::get);
                  triggerBRef.set(triggerB);
                  composedRef.set(composition.compose(triggerARef.get(), triggerB));
                });
          }
          case BOTH_DESCOPED ->
              enterScope(
                  ScopeType.COMMAND,
                  () -> {
                    var triggerA = new Trigger(m_scheduler, signalA::get);
                    var triggerB = new Trigger(m_scheduler, signalB::get);
                    triggerARef.set(triggerA);
                    triggerBRef.set(triggerB);
                    composedRef.set(composition.compose(triggerA, triggerB));
                  });
        };

    final var triggerA = triggerARef.get();
    final var triggerB = triggerBRef.get();
    final var composed = composedRef.get();

    m_scheduler.run();
    exitFirstScope.run();
    m_scheduler.run();

    assertEquals(
        !descopeCase.descopesA(),
        triggerA.isBound(),
        "Dependency A bound state mismatch after descoping");
    assertEquals(
        !descopeCase.descopesB(),
        triggerB.isBound(),
        "Dependency B bound state mismatch after descoping");
    assertFalse(composed.isBound(), "Composed trigger should be unbound after descoping");

    var runs = new AtomicLong();
    var command =
        Command.noRequirements(_ -> runs.incrementAndGet())
            .named("Binary re-scope command " + composition + " " + descopeCase);
    final var exitSecondScope = enterScope(ScopeType.OPMODE, () -> composed.onTrue(command));

    assertTrue(triggerA.isBound(), "Dependency A should be bound after composed trigger rebind");
    assertTrue(triggerB.isBound(), "Dependency B should be bound after composed trigger rebind");
    assertTrue(composed.isBound(), "Composed trigger should be bound in new scope");

    m_scheduler.run();
    assertEquals(0, runs.get(), "Composed trigger should not fire before stimulus");

    composition.stimulate(this, descopeCase, signalA, signalB, runs);

    assertEquals(
        1,
        runs.get(),
        "Re-scoped " + composition + " trigger did not fire for descope case " + descopeCase);

    exitSecondScope.run();
  }

  private static Stream<Arguments> unaryRescopeVariants() {
    return Stream.of(UnaryVariant.values()).map(Arguments::of);
  }

  private static Stream<Arguments> binaryRescopeVariants() {
    return Stream.of(BinaryComposition.values())
        .flatMap(
            composition ->
                Stream.of(DependencyDescopeCase.values())
                    .map(descopeCase -> Arguments.of(composition, descopeCase)));
  }

  private enum UnaryVariant {
    NEGATE,
    RISING_EDGE,
    FALLING_EDGE,
    DEBOUNCE,
    MULTI_PRESS;

    private boolean initialSignal() {
      return switch (this) {
        case NEGATE, FALLING_EDGE -> true;
        case RISING_EDGE, DEBOUNCE, MULTI_PRESS -> false;
      };
    }

    private Trigger derive(Trigger baseTrigger) {
      return switch (this) {
        case NEGATE -> baseTrigger.negate();
        case RISING_EDGE -> baseTrigger.risingEdge();
        case FALLING_EDGE -> baseTrigger.fallingEdge();
        case DEBOUNCE -> baseTrigger.debounce(Seconds.of(0.1));
        case MULTI_PRESS -> baseTrigger.multiPress(2, Seconds.of(1));
      };
    }

    private void stimulate(
        TriggerRescopeDependencyTest test, AtomicBoolean signal, AtomicLong currentTimeMicros) {
      switch (this) {
        case NEGATE, FALLING_EDGE -> {
          signal.set(false);
          test.m_scheduler.run();
        }
        case RISING_EDGE -> {
          signal.set(true);
          test.m_scheduler.run();
        }
        case DEBOUNCE -> {
          signal.set(true);
          test.m_scheduler.run();
          currentTimeMicros.addAndGet(200_000);
          test.m_scheduler.run();
        }
        case MULTI_PRESS -> {
          currentTimeMicros.set(1_100_000);
          signal.set(true);
          test.m_scheduler.run();

          signal.set(false);
          test.m_scheduler.run();

          currentTimeMicros.set(1_200_000);
          signal.set(true);
          test.m_scheduler.run();
        }
        default -> throw new IllegalStateException("Unexpected UnaryVariant " + this);
      }
    }
  }

  private enum BinaryComposition {
    OR,
    AND;

    private Trigger compose(Trigger triggerA, Trigger triggerB) {
      return switch (this) {
        case OR -> triggerA.or(triggerB);
        case AND -> triggerA.and(triggerB);
      };
    }

    private void stimulate(
        TriggerRescopeDependencyTest test,
        DependencyDescopeCase descopeCase,
        AtomicBoolean signalA,
        AtomicBoolean signalB,
        AtomicLong runs) {
      switch (this) {
        case OR -> {
          var signal =
              switch (descopeCase) {
                case A_DESCOPED_B_IN_SCOPE, BOTH_DESCOPED -> signalA;
                case A_IN_SCOPE_B_DESCOPED -> signalB;
              };
          signal.set(true);
          test.m_scheduler.run();
        }
        case AND -> {
          switch (descopeCase) {
            case A_DESCOPED_B_IN_SCOPE -> {
              signalB.set(true);
              test.m_scheduler.run();
              assertEquals(0, runs.get(), "AND trigger fired before descoped dependency A rose");

              signalA.set(true);
              test.m_scheduler.run();
            }
            case A_IN_SCOPE_B_DESCOPED, BOTH_DESCOPED -> {
              signalA.set(true);
              test.m_scheduler.run();
              assertEquals(0, runs.get(), "AND trigger fired before descoped dependency B rose");

              signalB.set(true);
              test.m_scheduler.run();
            }
            default -> throw new IllegalStateException("Unexpected descope case " + descopeCase);
          }
        }
        default -> throw new IllegalStateException("Unexpected composition " + this);
      }
    }
  }

  private enum DependencyDescopeCase {
    A_DESCOPED_B_IN_SCOPE,
    A_IN_SCOPE_B_DESCOPED,
    BOTH_DESCOPED;

    private boolean descopesA() {
      return this == A_DESCOPED_B_IN_SCOPE || this == BOTH_DESCOPED;
    }

    private boolean descopesB() {
      return this == A_IN_SCOPE_B_DESCOPED || this == BOTH_DESCOPED;
    }
  }
}
