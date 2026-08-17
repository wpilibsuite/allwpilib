// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicLong;
import java.util.concurrent.atomic.AtomicReference;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class TriggerRescopeTransitionTest extends TriggerRescopeTestBase {
  @ParameterizedTest(name = "two-scope transition {0} -> {1}")
  @MethodSource("allTwoScopeTransitions")
  void ensureBoundAcrossAllTwoScopeTransitions(ScopeType firstScope, ScopeType secondScope) {
    var signal = new AtomicBoolean(true);
    var triggerRef = new AtomicReference<Trigger>();

    final var exitFirstScope =
        enterScope(firstScope, () -> triggerRef.set(new Trigger(m_scheduler, signal::get)));
    final var trigger = triggerRef.get();
    m_scheduler.run();

    exitFirstScope.run();
    m_scheduler.run();

    if (firstScope != ScopeType.GLOBAL) {
      assertFalse(
          trigger.isBound(), "Trigger should unbind when exiting initial scope " + firstScope);

      signal.set(false);
      m_scheduler.run();
      assertTrue(
          trigger.getAsBoolean(),
          "Signal should remain frozen while trigger is unbound after " + firstScope);
    }

    final var ran = new AtomicBoolean(false);
    final var reboundCommand =
        Command.noRequirements(_ -> ran.set(true))
            .named("Rebound " + firstScope + " -> " + secondScope);
    final var exitSecondScope = enterScope(secondScope, () -> trigger.onTrue(reboundCommand));

    signal.set(false);
    m_scheduler.run();
    signal.set(true);
    m_scheduler.run();

    assertTrue(
        ran.get(),
        "Trigger did not fire after rebinding across transition "
            + firstScope
            + " -> "
            + secondScope);

    exitSecondScope.run();
  }

  @ParameterizedTest(name = "three-scope transition {0} -> {1} -> {2}")
  @MethodSource("allThreeScopeTransitions")
  void ensureBoundAcrossAllThreeScopeTransitions(
      ScopeType firstScope, ScopeType secondScope, ScopeType thirdScope) {
    var signal = new AtomicBoolean(true);
    var triggerRef = new AtomicReference<Trigger>();

    final var exitFirstScope =
        enterScope(firstScope, () -> triggerRef.set(new Trigger(m_scheduler, signal::get)));
    final var trigger = triggerRef.get();
    m_scheduler.run();

    exitFirstScope.run();
    m_scheduler.run();

    if (firstScope != ScopeType.GLOBAL) {
      assertFalse(
          trigger.isBound(), "Trigger should unbind after first scope " + firstScope + " exits");
    }

    final var secondScopeRuns = new AtomicLong(0);
    final var secondScopeCommand =
        Command.noRequirements(_ -> secondScopeRuns.incrementAndGet())
            .named("Second scope command " + secondScope);
    final var exitSecondScope = enterScope(secondScope, () -> trigger.onTrue(secondScopeCommand));

    signal.set(false);
    m_scheduler.run();
    signal.set(true);
    m_scheduler.run();
    assertEquals(
        1,
        secondScopeRuns.get(),
        "Second-scope binding did not run for transition " + firstScope + " -> " + secondScope);

    exitSecondScope.run();
    m_scheduler.run();

    if (firstScope != ScopeType.GLOBAL && secondScope != ScopeType.GLOBAL) {
      assertFalse(
          trigger.isBound(), "Trigger should unbind after second scope " + secondScope + " exits");
    }

    final var thirdScopeRuns = new AtomicLong(0);
    final var thirdScopeCommand =
        Command.noRequirements(_ -> thirdScopeRuns.incrementAndGet())
            .named("Third scope command " + thirdScope);
    final var exitThirdScope = enterScope(thirdScope, () -> trigger.onTrue(thirdScopeCommand));

    signal.set(false);
    m_scheduler.run();
    signal.set(true);
    m_scheduler.run();
    assertEquals(
        1,
        thirdScopeRuns.get(),
        "Third-scope binding did not run for transition "
            + firstScope
            + " -> "
            + secondScope
            + " -> "
            + thirdScope);

    var expectedSecondScopeRunsAfterThirdBind = secondScope == ScopeType.GLOBAL ? 2 : 1;
    assertEquals(
        expectedSecondScopeRunsAfterThirdBind,
        secondScopeRuns.get(),
        "Unexpected second-scope binding persistence for transition "
            + firstScope
            + " -> "
            + secondScope
            + " -> "
            + thirdScope);

    exitThirdScope.run();
  }

  private static Stream<Arguments> allTwoScopeTransitions() {
    return Stream.of(ScopeType.values())
        .flatMap(first -> Stream.of(ScopeType.values()).map(second -> Arguments.of(first, second)));
  }

  private static Stream<Arguments> allThreeScopeTransitions() {
    return Stream.of(ScopeType.values())
        .flatMap(
            first ->
                Stream.of(ScopeType.values())
                    .flatMap(
                        second ->
                            Stream.of(ScopeType.values())
                                .map(third -> Arguments.of(first, second, third))));
  }
}
