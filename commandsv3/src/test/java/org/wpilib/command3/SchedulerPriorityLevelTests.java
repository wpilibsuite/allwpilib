// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.MethodSource;
import org.wpilib.command3.Scheduler.ScheduleResult.LowerPriorityThanRunningCommand;

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

  @Test
  void childInheritsHigherParentPriority() {
    var mech = new DummyMechanism("Mechanism", m_scheduler);

    var higherPriority = new PriorityCommand(200, mech);
    var lowPriorityChild = new PriorityCommand(-1000, mech);
    var parent =
        Command.noRequirements(
                coroutine -> {
                  coroutine.await(lowPriorityChild);
                })
            .withPriority(1000)
            .named("Parent");

    m_scheduler.schedule(higherPriority);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(parent), "Parent command should be running");
    assertTrue(m_scheduler.isRunning(lowPriorityChild), "Child command should be running");
    assertFalse(
        m_scheduler.isRunning(higherPriority),
        "Higher priority command should have been interrupted");
  }

  @Test
  void innerDefaultCommandInheritsHigherParentPriority() {
    var mech = new DummyMechanism("Mechanism", m_scheduler);

    final Command higherPriority = new PriorityCommand(200, mech);
    final Command lowPriorityChild = new PriorityCommand(-1000, mech);
    final Command parent =
        Command.noRequirements(
                coroutine -> {
                  mech.setDefaultCommand(lowPriorityChild);
                  coroutine.park();
                })
            .withPriority(1000)
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(List.of(parent, lowPriorityChild), m_scheduler.getRunningCommands());

    // the inner default command should inherit priority=1000 and prevent
    // the 200-priority command from running
    var result = m_scheduler.schedule(higherPriority);
    var failure = assertInstanceOf(LowerPriorityThanRunningCommand.class, result);
    assertSame(lowPriorityChild, failure.alreadyRunning());
    assertSame(higherPriority, failure.command());
    assertEquals(
        List.of(parent, lowPriorityChild),
        m_scheduler.getRunningCommands(),
        "Parent and the inner default command should continue to run");
  }

  @Test
  void innerTriggerCommandInheritsHigherParentPriority() {
    var mech = new DummyMechanism("Mechanism", m_scheduler);

    var higherPriority = new PriorityCommand(200, mech);
    var lowPriorityChild = new PriorityCommand(-1000, mech);

    var trigger = new Trigger(m_scheduler, () -> true);
    var parent =
        Command.noRequirements(
                coroutine -> {
                  // Because the trigger is created outside this command and before it's scheduled,
                  // an `onTrue` or `whileTrue` binding won't fire because the signal edge happens
                  // before the binding can be evaluated.
                  trigger.retryWhileTrue(lowPriorityChild);

                  coroutine.park();
                })
            .withPriority(1000)
            .named("Parent");

    m_scheduler.schedule(higherPriority);
    m_scheduler.run();

    m_scheduler.schedule(parent);
    m_scheduler.run(); // Schedules parent, adds binding. The binding is evaluated on the next run()
    m_scheduler.run(); // Polls the binding and schedules the child

    assertEquals(
        List.of(parent, lowPriorityChild),
        m_scheduler.getRunningCommands(),
        "Trigger-bound child should have inherited parent's priority");
    assertSchedulerEvent(
        SchedulerEvent.Interrupted.class,
        i -> {
          return i.interrupter().equals(lowPriorityChild) && i.command().equals(higherPriority);
        },
        "Higher-priority command should have been interrupted by the trigger-bound child");
  }

  @Test
  void conflictingCommandsWithPriorityInheritance() {
    var mech = new DummyMechanism("Mechanism", m_scheduler);

    var child1 = new PriorityCommand(0, mech);
    var child2 = new PriorityCommand(0, mech);

    var parent1 =
        Command.noRequirements(
                coroutine -> {
                  coroutine.await(child1);
                })
            .withPriority(2000)
            .named("Parent1");

    var parent2 =
        Command.noRequirements(
                coroutine -> {
                  // child2 inherits parent2 priority (1000) and should fail to be scheduled due to
                  // being a lower priority than child1 (2000, inherited from its parent)
                  coroutine.await(child2);
                })
            .withPriority(1000)
            .named("Parent2");

    m_scheduler.schedule(parent1);
    m_scheduler.schedule(parent2);
    m_scheduler.run();

    assertEquals(List.of(parent1, child1), m_scheduler.getRunningCommands());
  }

  @Test
  void conflictingCommandsWithPriorityInheritance2() {
    var mech = new DummyMechanism("Mechanism", m_scheduler);

    var child1 = new PriorityCommand(0, mech);
    var child2 = new PriorityCommand(0, mech);

    var parent1 =
        Command.noRequirements(
                coroutine -> {
                  coroutine.await(child1);
                })
            .withPriority(1000)
            .named("Parent1");

    var parent2 =
        Command.noRequirements(
                coroutine -> {
                  // child2 inherits parent2 priority (2000) and should be scheduled due to being a
                  // higher priority than child1 (1000, inherited from its parent)
                  coroutine.await(child2);
                })
            .withPriority(2000)
            .named("Parent2");

    m_scheduler.schedule(parent1);
    m_scheduler.schedule(parent2);
    m_scheduler.run();

    assertEquals(List.of(parent2, child2), m_scheduler.getRunningCommands());
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
    // Only the command that failed to fork should get an interrupted event.
    // All other commands in the composition will still be canceled, but won't be interrupted.
    assertInterruptedBy(current, highPriority);
    for (var command : List.of(grandparent, parent, current, child, grandchild)) {
      assertFalse(
          m_scheduler.isScheduledOrRunning(command), command.name() + " should have been canceled");
    }
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("multiChildForkOperations")
  void highPriorityGrandchildWithLowPriorityChild(NamedCoroutineForkOperation operation) {
    /*
     Given this setup, child1 and child2 are both forkable at the same time (no shared requirements
     and no conflicts with running commands), but child1 forks a higher-priority command than
     child2, which should cause child2 to fail to be scheduled

     parent:
       child1:
         fork priority(1000, mech)
       child2: priority(0, mech))
    */

    var mech = new DummyMechanism("Mech", m_scheduler);

    PriorityCommand grandchild = new PriorityCommand(1000, mech);
    var child1 = Command.noRequirements(co -> co.await(grandchild)).named("Child1");
    var child2 = new PriorityCommand(0, mech);

    var parent =
        Command.noRequirements(
                co -> {
                  co.setCancelOnForkFailure(false);
                  var failure = operation.operation().apply(co, child1, child2);
                  assertTrue(failure.failed(), "Forking operation should have failed");

                  var fails = failure.getFailedCommands();
                  assertEquals(1, fails.size(), "Exactly one failure was expected");
                  assertInstanceOf(
                      LowerPriorityThanRunningCommand.class,
                      fails.getFirst(),
                      "Failure should be a LowerPriorityThanRunningCommand");

                  var failedCommand = fails.getFirst().command();
                  var running =
                      ((LowerPriorityThanRunningCommand) fails.getFirst()).alreadyRunning();
                  assertEquals(child2, failedCommand, "Failed command should be child2");
                  assertEquals(grandchild, running, "Running command should be grandchild");
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
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
    assertTrue(result.failed(), "Fork result should be a failure");

    var failedCommands =
        result.getFailedCommands().stream().map(Scheduler.ScheduleResult::command).toList();
    assertEquals(expectedFailedCommands, failedCommands);
    assertTrue(
        result.getFailedCommands().stream().noneMatch(Scheduler.ScheduleResult::successful),
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
