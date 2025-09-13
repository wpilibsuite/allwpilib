// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.Supplier;
import org.junit.jupiter.api.Test;

class SchedulerConflictTests extends CommandTestBase {
  @Test
  void compositionsCannotAwaitConflictingCommands() {
    var mech = new Mechanism("The Mechanism", m_scheduler);

    var group =
        Command.noRequirements()
            .executing(
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
  void siblingsInCompositionCanShareRequirements() {
    var mechanism = new Mechanism("The mechanism", m_scheduler);
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
        Command.noRequirements()
            .executing(
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
        () -> Command.noRequirements().executing(_c -> runs.incrementAndGet()).named("One Shot");
    var command =
        Command.noRequirements()
            .executing(
                co -> {
                  co.fork(makeOneShot.get());
                  co.fork(makeOneShot.get());
                  co.fork(
                      Command.noRequirements()
                          .executing(inner -> inner.fork(makeOneShot.get()))
                          .named("Inner"));
                  co.fork(
                      Command.noRequirements()
                          .executing(
                              co2 -> {
                                co2.fork(makeOneShot.get());
                                co2.fork(
                                    Command.noRequirements()
                                        .executing(
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
    var mechanism = new Mechanism("mechanism", m_scheduler);
    var top = mechanism.run(Coroutine::park).withPriority(10).named("Top");

    // Child conflicts with and is lower priority than the Top command
    // It should not be scheduled, and the parent command should exit immediately
    var child = mechanism.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements().executing(co -> co.await(child)).named("Parent");

    m_scheduler.schedule(top);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertTrue(m_scheduler.isRunning(top), "Top command should not have been interrupted");
    assertFalse(m_scheduler.isRunning(child), "Conflicting child should not have run");
    assertFalse(m_scheduler.isRunning(parent), "Parent of conflicting child should have exited");
  }

  @Test
  void childConflictsWithLowerPriorityTopLevel() {
    var mechanism = new Mechanism("mechanism", m_scheduler);
    var top = mechanism.run(Coroutine::park).withPriority(-10).named("Top");

    // Child conflicts with and is lower priority than the Top command
    // It should not be scheduled, and the parent command should exit immediately
    var child = mechanism.run(Coroutine::park).named("Child");
    var parent = Command.noRequirements().executing(co -> co.await(child)).named("Parent");

    m_scheduler.schedule(top);
    m_scheduler.schedule(parent);
    m_scheduler.run();

    assertFalse(m_scheduler.isRunning(top), "Top command should have been interrupted");
    assertTrue(m_scheduler.isRunning(child), "Conflicting child should be running");
    assertTrue(m_scheduler.isRunning(parent), "Parent of conflicting child should be running");
  }
}
