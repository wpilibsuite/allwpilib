// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;

class SchedulerPriorityLevelTests extends CommandTestBase {
  @FunctionalInterface
  private interface CoroutineForkOperation {
    Coroutine.ForkResult apply(Coroutine coroutine, Command... commands);
  }

  private record NamedCoroutineForkOperation(String name, CoroutineForkOperation operation) {
    @Override
    public String toString() {
      return name;
    }
  }

  private static Stream<NamedCoroutineForkOperation> singleChildForkOperations() {
    return Stream.of(
        new NamedCoroutineForkOperation("fork", Coroutine::fork),
        new NamedCoroutineForkOperation(
            "await", (coroutine, commands) -> coroutine.await(commands[0])),
        new NamedCoroutineForkOperation("awaitAll", Coroutine::awaitAll),
        new NamedCoroutineForkOperation("awaitAny", Coroutine::awaitAny));
  }

  private static Stream<NamedCoroutineForkOperation> multiChildForkOperations() {
    return Stream.of(
        new NamedCoroutineForkOperation("fork", Coroutine::fork),
        new NamedCoroutineForkOperation("awaitAll", Coroutine::awaitAll),
        new NamedCoroutineForkOperation("awaitAny", Coroutine::awaitAny));
  }

  @Test
  void higherPriorityCancels() {
    final var subsystem = new DummyMechanism("Subsystem", m_scheduler);

    final var lower = new PriorityCommand(-1000, subsystem);
    final var higher = new PriorityCommand(+1000, subsystem);

    m_scheduler.schedule(lower);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(lower));

    m_scheduler.schedule(higher);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(higher));
    assertFalse(m_scheduler.isRunning(lower));
  }

  @Test
  void lowerPriorityDoesNotCancel() {
    final var subsystem = new DummyMechanism("Subsystem", m_scheduler);

    final var lower = new PriorityCommand(-1000, subsystem);
    final var higher = new PriorityCommand(+1000, subsystem);

    m_scheduler.schedule(higher);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(higher));

    m_scheduler.schedule(lower);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(higher), "Higher priority command should still be running");
    assertFalse(
        m_scheduler.isScheduledOrRunning(lower), "Lower priority command should not be running");
  }

  @Test
  void samePriorityCancels() {
    final var subsystem = new DummyMechanism("Subsystem", m_scheduler);

    final var first = new PriorityCommand(512, subsystem);
    final var second = new PriorityCommand(512, subsystem);

    m_scheduler.schedule(first);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(first));

    m_scheduler.schedule(second);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(second), "New command should be running");
    assertFalse(m_scheduler.isRunning(first), "Old command should be canceled");
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("singleChildForkOperations")
  void lowPriorityChildCancelsParent(NamedCoroutineForkOperation operation) {
    assertUnschedulableSingleChildCancelsParent(operation.operation());
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("singleChildForkOperations")
  void lowPriorityChildGivesFailureObject(NamedCoroutineForkOperation operation) {
    assertUnschedulableSingleChildReturnsFailure(operation.operation());
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("multiChildForkOperations")
  void lowPriorityChildWithSchedulableSiblingGivesFailureObject(
      NamedCoroutineForkOperation operation) {
    assertUnschedulableChildWithSchedulableSiblingReturnsFailure(operation.operation());
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("singleChildForkOperations")
  void lowPriorityChildCancelsEntireComposition(NamedCoroutineForkOperation operation) {
    final var subsystem = new DummyMechanism("Subsystem", m_scheduler);

    var highPriority = new PriorityCommand(512, subsystem);
    var unschedulable = new PriorityCommand(Command.DEFAULT_PRIORITY, subsystem);
    var grandchild = Command.noRequirements(Coroutine::park).named("Grandchild");
    var child =
        Command.noRequirements(
                coroutine -> {
                  coroutine.fork(grandchild);
                  coroutine.park();
                })
            .named("Child");
    var current =
        Command.noRequirements(
                coroutine -> {
                  coroutine.fork(child);
                  operation.operation().apply(coroutine, unschedulable);
                })
            .named("Current");
    var parent = Command.noRequirements(coroutine -> coroutine.await(current)).named("Parent");
    var grandparent =
        Command.noRequirements(coroutine -> coroutine.await(parent)).named("Grandparent");

    m_scheduler.schedule(highPriority);
    m_scheduler.schedule(grandparent);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(highPriority), "Higher priority command should still run");
    for (var command : List.of(grandparent, parent, current, child, grandchild)) {
      assertFalse(
          m_scheduler.isScheduledOrRunning(command),
          command.name() + " should have been canceled");
      assertInterruptedBy(command, highPriority);
    }
  }

  private void assertUnschedulableSingleChildCancelsParent(CoroutineForkOperation operation) {
    final var subsystem = new DummyMechanism("Subsystem", m_scheduler);

    var highPriority = new PriorityCommand(512, subsystem);
    var defaultPriority = new PriorityCommand(Command.DEFAULT_PRIORITY, subsystem);
    var parent =
        Command.noRequirements(coroutine -> operation.apply(coroutine, defaultPriority))
            .named("Parent");

    m_scheduler.schedule(highPriority);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertFalse(m_scheduler.isRunning(parent), "Parent command should have been canceled");
    assertSchedulerEvent(
        SchedulerEvent.Canceled.class,
        c -> c.command().equals(parent),
        "Should have received a Canceled event for parent");
    assertSchedulerEvent(
        SchedulerEvent.Interrupted.class,
        i -> i.command().equals(parent) && i.interrupter().equals(highPriority),
        "Should have received an Interrupted event for parent");
  }

  private void assertUnschedulableSingleChildReturnsFailure(CoroutineForkOperation operation) {
    final var subsystem = new DummyMechanism("Subsystem", m_scheduler);

    var highPriority = new PriorityCommand(512, subsystem);
    var defaultPriority = new PriorityCommand(Command.DEFAULT_PRIORITY, subsystem);

    assertForkFailure(operation, highPriority, List.of(defaultPriority), List.of(defaultPriority));
  }

  private void assertUnschedulableChildWithSchedulableSiblingReturnsFailure(
      CoroutineForkOperation operation) {
    final var busySubsystem = new DummyMechanism("Busy Subsystem", m_scheduler);
    final var idleSubsystem = new DummyMechanism("Idle Subsystem", m_scheduler);

    var highPriority = new PriorityCommand(512, busySubsystem);
    var unschedulable = new PriorityCommand(Command.DEFAULT_PRIORITY, busySubsystem);
    var schedulable = new PriorityCommand(Command.DEFAULT_PRIORITY, idleSubsystem);

    assertForkFailure(
        operation, highPriority, List.of(unschedulable, schedulable), List.of(unschedulable));
  }

  private void assertForkFailure(
      CoroutineForkOperation operation,
      Command alreadyRunning,
      List<Command> commands,
      List<Command> expectedFailures) {
    var parent =
        Command.noRequirements(
                coroutine -> {
                  coroutine.setCancelOnForkFailure(false);
                  var result = operation.apply(coroutine, commands.toArray(Command[]::new));
                  assertFailureResult(result, expectedFailures);

                  coroutine.park();
                })
            .named("Parent");

    m_scheduler.schedule(alreadyRunning);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(parent), "Parent command should still be running");
    assertTrue(
        m_scheduler.isRunning(alreadyRunning), "Higher priority command should still be running");
    for (var command : commands) {
      assertFalse(
          m_scheduler.isScheduledOrRunning(command),
          command.name() + " should not have been scheduled");
    }
  }

  private static void assertFailureResult(
      Coroutine.ForkResult result, List<Command> expectedFailedCommands) {
    assertInstanceOf(Coroutine.ForkResultFailure.class, result, "Fork result should be a failure");
    var failure = (Coroutine.ForkResultFailure) result;

    var failedCommands = failure.failed().stream().map(Scheduler.ScheduleResult::command).toList();
    assertEquals(expectedFailedCommands, failedCommands);
    assertTrue(
        failure.failed().stream().noneMatch(Scheduler.ScheduleResult::successful),
        "All failure results should be unsuccessful");
  }

  private void assertInterruptedBy(Command command, Command interrupter) {
    assertSchedulerEvent(
        SchedulerEvent.Canceled.class,
        event -> event.command().equals(command),
        command.name() + " should have received a Canceled event");
    assertSchedulerEvent(
        SchedulerEvent.Interrupted.class,
        event -> event.command().equals(command) && event.interrupter().equals(interrupter),
        command.name() + " should have received an Interrupted event");
  }
}
