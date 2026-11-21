// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Supplier;
import org.junit.jupiter.api.Test;

@SuppressWarnings("PMD.CompareObjectsWithEquals")
class SchedulerConflictTests extends CommandTestBase {
  @Test
  void compositionsCannotAwaitConflictingCommands() {
    var mech = new DummyMechanism("The Mechanism", m_scheduler);

    var group =
        Command.noRequirements(
                co -> {
                  co.awaitAll(
                      mech.run(Coroutine::park).named("First"),
                      mech.run(Coroutine::park).named("Second"));
                })
            .named("Group");

    m_scheduler.schedule(group);

    // Running should attempt to schedule multiple conflicting commands
    var exception = assertThrows(IllegalArgumentException.class, m_scheduler::run);
    assertEquals(
        "Commands running in parallel cannot share requirements: "
            + "First and Second both require The Mechanism",
        exception.getMessage());
  }

  @Test
  void innerCommandMayInterruptOtherInnerCommand() {
    var mechanism = new DummyMechanism("The mechanism", m_scheduler);
    var firstRan = new AtomicBoolean(false);
    var secondRan = new AtomicBoolean(false);

    var first =
        mechanism
            .run(
                c -> {
                  firstRan.set(true);
                  c.park();
                })
            .named("First");

    var second =
        mechanism
            .run(
                c -> {
                  secondRan.set(true);
                  c.park();
                })
            .named("Second");

    var group =
        Command.noRequirements(
                co -> {
                  co.fork(first);
                  co.fork(second);
                  co.park();
                })
            .named("Group");

    m_scheduler.schedule(group);
    m_scheduler.run();

    assertTrue(firstRan.get(), "First child should have run to a yield point");
    assertTrue(secondRan.get(), "Second child should have run to a yield point");
    assertFalse(
        m_scheduler.isScheduledOrRunning(first), "First child should have been interrupted");
    assertTrue(m_scheduler.isRunning(second), "Second child should still be running");
    assertTrue(m_scheduler.isRunning(group), "Group should still be running");
  }

  @Test
  void nestedOneShotCompositionsAllRunInOneCycle() {
    var runs = new AtomicInteger(0);
    Supplier<Command> makeOneShot =
        () -> Command.noRequirements(_c -> runs.incrementAndGet()).named("One Shot");
    var command =
        Command.noRequirements(
                co -> {
                  co.fork(makeOneShot.get());
                  co.fork(makeOneShot.get());
                  co.fork(
                      Command.noRequirements(inner -> inner.fork(makeOneShot.get()))
                          .named("Inner"));
                  co.fork(
                      Command.noRequirements(
                              co2 -> {
                                co2.fork(makeOneShot.get());
                                co2.fork(
                                    Command.noRequirements(
                                            co3 -> {
                                              co3.fork(makeOneShot.get());
                                            })
                                        .named("3"));
                              })
                          .named("2"));
                })
            .named("Command");

    m_scheduler.schedule(command);
    m_scheduler.run();
    assertEquals(5, runs.get(), "All oneshot commands should have run");
    assertFalse(m_scheduler.isRunning(command), "Command should have exited after one cycle");
  }

  @Test
  void childConflictsWithHigherPriorityTopLevel() {
    var mechanism = new DummyMechanism("mechanism", m_scheduler);
    var top = mechanism.run(Coroutine::park).withPriority(10).named("Top");

    // Child conflicts with and is lower priority than the Top command
    // It should not be scheduled, and the parent command should exit immediately
    var child = mechanism.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements(co -> co.await(child)).named("Parent");

    m_scheduler.schedule(top);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(top), "Top command should not have been interrupted");
    assertFalse(m_scheduler.isRunning(child), "Conflicting child should not have run");
    assertFalse(m_scheduler.isRunning(parent), "Parent of conflicting child should have exited");
  }

  @Test
  void childConflictsWithLowerPriorityTopLevel() {
    var mechanism = new DummyMechanism("mechanism", m_scheduler);
    var top = mechanism.run(Coroutine::park).withPriority(-10).named("Top");

    // Child conflicts with and is higher priority than the Top command
    // It should be scheduled, and the top command should be interrupted
    var child = mechanism.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements(co -> co.await(child)).named("Parent");

    m_scheduler.schedule(top);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertFalse(m_scheduler.isRunning(top), "Top command should have been interrupted");
    assertTrue(m_scheduler.isRunning(child), "Conflicting child should be running");
    assertTrue(m_scheduler.isRunning(parent), "Parent of conflicting child should be running");
  }

  @Test
  void niblingInterruptsDirectChild() {
    var mech = new DummyMechanism("Mech", m_scheduler);
    var child = mech.run(Coroutine::park).named("Child");
    var nibling = mech.run(Coroutine::park).named("Nibling");
    var inner = Command.noRequirements(co -> co.await(nibling)).named("Inner");

    // Parent
    //   -> Child (requires mech)
    //   -> Inner
    //      -> Nibling (requires mech)
    // "Nibling" should interrupt "Child"
    var parent =
        Command.noRequirements(
                co -> {
                  co.fork(child);
                  co.yield();

                  co.fork(inner);
                  co.park();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(List.of(parent, child), m_scheduler.getRunningCommands());

    m_scheduler.run();
    assertEquals(List.of(parent, inner, nibling), m_scheduler.getRunningCommands());
    assertSchedulerEvent(
        SchedulerEvent.Interrupted.class,
        e -> {
          return e.interrupter() == nibling && e.command() == child;
        },
        "Nibling should have interrupted Child");
  }

  @Test
  void directChildInterruptsNibling() {
    var mech = new DummyMechanism("Mech", m_scheduler);
    var child = mech.run(Coroutine::park).named("Child");
    var nibling = mech.run(Coroutine::park).named("Nibling");
    var inner = Command.noRequirements(co -> co.await(nibling)).named("Inner");

    // Parent
    //   -> Inner
    //      -> Nibling (requires mech)
    //   -> Child (requires mech)
    // "Child" conflicts with "Nibling" and interrupts "Inner"
    var parent =
        Command.noRequirements(
                co -> {
                  co.fork(inner);
                  co.yield();

                  co.fork(child);
                  co.park();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(List.of(parent, inner, nibling), m_scheduler.getRunningCommands());

    m_scheduler.run();
    assertEquals(List.of(parent, child), m_scheduler.getRunningCommands());
    assertSchedulerEvent(
        SchedulerEvent.Interrupted.class,
        e -> {
          return e.interrupter() == child && e.command() == inner;
        },
        "Child should have interrupted Inner");
  }

  @Test
  void cousinsCancel() {
    var mech = new DummyMechanism("Mech", m_scheduler);
    var cousin1 = mech.run(Coroutine::park).named("Cousin 1");
    var cousin2 = mech.run(Coroutine::park).named("Cousin 2");
    var child1 = Command.noRequirements(co -> co.await(cousin1)).named("Child 1");
    var child2 = Command.noRequirements(co -> co.await(cousin2)).named("Child 2");
    var parent =
        Command.noRequirements(
                co -> {
                  co.fork(child1);
                  co.yield();
                  co.fork(child2);
                  co.park();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertEquals(List.of(parent, child1, cousin1), m_scheduler.getRunningCommands());

    m_scheduler.run();
    assertEquals(List.of(parent, child2, cousin2), m_scheduler.getRunningCommands());

    m_scheduler.run();
    assertEquals(List.of(parent, child2, cousin2), m_scheduler.getRunningCommands());
  }

  @Test
  void longDistanceCousinsInterrupt() {
    var mech = new DummyMechanism("Mech", m_scheduler);
    var cousin1 = mech.run(Coroutine::park).named("Cousin 1");
    var cousin2 = mech.run(Coroutine::park).named("Cousin 2");

    // Parent -> Child1.0 -> ... -> Child1.50 -> Cousin 1
    //        -> Child2.0 -> ... -> Child2.100 -> Cousin 2
    // Cousin 2 conflicts with Cousin 1 and interrupts Child1.050
    var parent =
        Command.noRequirements(
                co -> {
                  var chainHead1 = cousin1;
                  for (int i = 50; i >= 0; i--) {
                    var localChild = chainHead1;
                    chainHead1 =
                        Command.noRequirements(c -> c.await(localChild)).named("Child1." + i);
                  }
                  co.fork(chainHead1);
                  co.yield();

                  var chainHead2 = cousin2;
                  for (int i = 100; i >= 0; i--) {
                    var localChild = chainHead2;
                    chainHead2 =
                        Command.noRequirements(c -> c.await(localChild)).named("Child2." + i);
                  }
                  co.fork(chainHead2);
                  co.park();
                })
            .named("Parent");

    m_scheduler.schedule(parent);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(parent), "Parent command should be running");
    assertTrue(m_scheduler.isRunning(cousin1), "Cousin 1 should be running");
    assertFalse(m_scheduler.isRunning(cousin2), "Cousin 2 should not be running yet");

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(parent), "Parent command should still be running");
    assertFalse(m_scheduler.isRunning(cousin1), "Cousin 1 should have been interrupted");
    assertTrue(m_scheduler.isRunning(cousin2), "Cousin 2 should have started");
    assertSchedulerEvent(
        SchedulerEvent.Interrupted.class,
        e -> {
          return e.interrupter() == cousin2 && "Child1.0".equals(e.command().name());
        },
        "Cousin 2 should have interrupted the chain root for Cousin 1");

    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(parent), "Parent command should still be running");
    assertTrue(m_scheduler.isRunning(cousin2), "Cousin 2 should still be running");
  }

  @Test
  void nestedConflictingBranchesEviction() {
    var mech = new DummyMechanism("Mech", m_scheduler);
    var leafA = mech.run(Coroutine::park).named("LeafA");
    var branchA =
        Command.noRequirements(
                co -> {
                  co.fork(leafA);
                  co.park();
                })
            .named("BranchA");

    var leafB = mech.run(Coroutine::park).named("LeafB");
    var branchB =
        Command.noRequirements(
                co -> {
                  co.fork(leafB);
                  co.park();
                })
            .named("BranchB");

    var root =
        Command.noRequirements(
                co -> {
                  co.fork(branchA);
                  co.fork(branchB);
                  co.park();
                })
            .named("Root");

    m_scheduler.schedule(root);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(root), "Root command should still be running");
    assertTrue(m_scheduler.isRunning(branchB), "BranchB should still be running");
    assertTrue(m_scheduler.isRunning(leafB), "LeafB should still be running");
    assertFalse(m_scheduler.isRunning(branchA), "BranchA should have been canceled");
    assertFalse(m_scheduler.isRunning(leafA), "LeafA should have been canceled");
  }

  @Test
  void deeplyNestedUnrelatedCommandEviction() {
    var mech = new DummyMechanism("Mech", m_scheduler);
    var leaf1 = mech.run(Coroutine::park).named("Leaf1");
    var parent1 =
        Command.noRequirements(
                co -> {
                  co.fork(leaf1);
                  co.park();
                })
            .named("Parent1");
    var root1 =
        Command.noRequirements(
                co -> {
                  co.fork(parent1);
                  co.park();
                })
            .named("Root1");

    m_scheduler.schedule(root1);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(root1));
    assertTrue(m_scheduler.isRunning(parent1));
    assertTrue(m_scheduler.isRunning(leaf1));

    var leaf2 = mech.run(Coroutine::park).named("Leaf2");
    var parent2 =
        Command.noRequirements(
                co -> {
                  co.fork(leaf2);
                  co.park();
                })
            .named("Parent2");
    var root2 =
        Command.noRequirements(
                co -> {
                  co.fork(parent2);
                  co.park();
                })
            .named("Root2");

    m_scheduler.schedule(root2);
    m_scheduler.run();

    assertFalse(m_scheduler.isRunning(root1), "Root1 should have been canceled");
    assertFalse(m_scheduler.isRunning(parent1), "Parent1 should have been canceled");
    assertFalse(m_scheduler.isRunning(leaf1), "Leaf1 should have been canceled");

    assertTrue(m_scheduler.isRunning(root2), "Root2 should be running");
    assertTrue(m_scheduler.isRunning(parent2), "Parent2 should be running");
    assertTrue(m_scheduler.isRunning(leaf2), "Leaf2 should be running");
  }
}
