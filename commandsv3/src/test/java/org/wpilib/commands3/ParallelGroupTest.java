// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.Set;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class ParallelGroupTest extends CommandTestBase {
  @Test
  void parallelAll() {
    var r1 = Mechanism.createDummy("R1", m_scheduler);
    var r2 = Mechanism.createDummy("R2", m_scheduler);

    var c1Count = new AtomicInteger(0);
    var c2Count = new AtomicInteger(0);

    var c1 =
        r1.run(
                coroutine -> {
                  for (int i = 0; i < 5; i++) {
                    coroutine.yield();
                    c1Count.incrementAndGet();
                  }
                })
            .named("C1");
    var c2 =
        r2.run(
                coroutine -> {
                  for (int i = 0; i < 10; i++) {
                    coroutine.yield();
                    c2Count.incrementAndGet();
                  }
                })
            .named("C2");

    var parallel = new ParallelGroup("Parallel", List.of(c1, c2), List.of());
    m_scheduler.schedule(parallel);

    // First call to run() should schedule and start the commands
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(parallel));
    assertTrue(m_scheduler.isRunning(c1));
    assertTrue(m_scheduler.isRunning(c2));

    // Next call to run() should start them
    for (int i = 1; i < 5; i++) {
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(c1));
      assertTrue(m_scheduler.isRunning(c2));
      assertEquals(i, c1Count.get());
      assertEquals(i, c2Count.get());
    }
    // c1 should finish after 5 iterations; c2 should continue for another 5
    for (int i = 5; i < 10; i++) {
      m_scheduler.run();
      assertFalse(m_scheduler.isRunning(c1));
      assertTrue(m_scheduler.isRunning(c2));
      assertEquals(5, c1Count.get());
      assertEquals(i, c2Count.get());
    }

    // one final run() should unschedule the c2 command and end the group
    assertTrue(m_scheduler.isRunning(parallel));
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(c1));
    assertFalse(m_scheduler.isRunning(c2));
    assertFalse(m_scheduler.isRunning(parallel));

    // and final counts should be 5 and 10
    assertEquals(5, c1Count.get());
    assertEquals(10, c2Count.get());
  }

  @Test
  void race() {
    var r1 = Mechanism.createDummy("R1", m_scheduler);
    var r2 = Mechanism.createDummy("R2", m_scheduler);

    var c1Count = new AtomicInteger(0);
    var c2Count = new AtomicInteger(0);

    var c1 =
        r1.run(
                coroutine -> {
                  for (int i = 0; i < 5; i++) {
                    coroutine.yield();
                    c1Count.incrementAndGet();
                  }
                })
            .named("C1");
    var c2 =
        r2.run(
                coroutine -> {
                  for (int i = 0; i < 10; i++) {
                    coroutine.yield();
                    c2Count.incrementAndGet();
                  }
                })
            .named("C2");

    var race = new ParallelGroup("Race", List.of(), List.of(c1, c2));
    m_scheduler.schedule(race);

    // First call to run() should schedule the commands
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(race));
    assertTrue(m_scheduler.isRunning(c1));
    assertTrue(m_scheduler.isRunning(c2));

    for (int i = 1; i < 5; i++) {
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(c1));
      assertTrue(m_scheduler.isRunning(c2));
      assertEquals(i, c1Count.get());
      assertEquals(i, c2Count.get());
    }
    m_scheduler.run(); // complete c1
    assertFalse(m_scheduler.isRunning(race));
    assertFalse(m_scheduler.isRunning(c1));
    assertFalse(m_scheduler.isRunning(c2));

    // and final counts should be 5 and 5
    assertEquals(5, c1Count.get());
    assertEquals(5, c2Count.get());
  }

  @Test
  void nested() {
    var mechanism = Mechanism.createDummy("mechanism", m_scheduler);

    var count = new AtomicInteger(0);

    var command =
        mechanism
            .run(
                coroutine -> {
                  for (int i = 0; i < 5; i++) {
                    coroutine.yield();
                    count.incrementAndGet();
                  }
                })
            .named("Command");

    var inner = new ParallelGroup("Inner", Set.of(command), Set.of());
    var outer = new ParallelGroup("Outer", Set.of(), Set.of(inner));

    // Scheduling: Outer group should be on deck
    m_scheduler.schedule(outer);
    assertTrue(m_scheduler.isScheduled(outer));
    assertFalse(m_scheduler.isScheduledOrRunning(inner));
    assertFalse(m_scheduler.isScheduledOrRunning(command));

    // First run: Inner group and command should both be scheduled and running
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(outer), "Outer group should be running");
    assertTrue(m_scheduler.isRunning(inner), "Inner group should be running");
    assertTrue(m_scheduler.isRunning(command), "Command should be running");
    assertEquals(0, count.get());

    // Runs 2 through 5: Outer and inner should both be running while the command runs
    for (int i = 1; i < 5; i++) {
      m_scheduler.run();
      assertTrue(m_scheduler.isRunning(outer), "Outer group should be running");
      assertTrue(m_scheduler.isRunning(inner), "Inner group should be running");
      assertTrue(m_scheduler.isRunning(command), "Command should be running (" + i + ")");
      assertEquals(i, count.get());
    }

    // Run 6: Command should have completed naturally
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(outer), "Outer group should be running");
    assertFalse(m_scheduler.isRunning(inner), "Inner group should be running");
    assertFalse(m_scheduler.isRunning(command), "Command should have completed");
  }

  @Test
  void automaticNameRace() {
    var a = Command.noRequirements().executing(coroutine -> {}).named("A");
    var b = Command.noRequirements().executing(coroutine -> {}).named("B");
    var c = Command.noRequirements().executing(coroutine -> {}).named("C");

    var group = new ParallelGroupBuilder().optional(a, b, c).withAutomaticName();
    assertEquals("(A | B | C)", group.name());
  }

  @Test
  void automaticNameAll() {
    var a = Command.noRequirements().executing(coroutine -> {}).named("A");
    var b = Command.noRequirements().executing(coroutine -> {}).named("B");
    var c = Command.noRequirements().executing(coroutine -> {}).named("C");

    var group = new ParallelGroupBuilder().requiring(a, b, c).withAutomaticName();
    assertEquals("(A & B & C)", group.name());
  }

  @Test
  void automaticNameDeadline() {
    var a = Command.noRequirements().executing(coroutine -> {}).named("A");
    var b = Command.noRequirements().executing(coroutine -> {}).named("B");
    var c = Command.noRequirements().executing(coroutine -> {}).named("C");

    var group = new ParallelGroupBuilder().requiring(a).optional(b, c).withAutomaticName();
    assertEquals("[(A) * (B | C)]", group.name());
  }

  @Test
  void inheritsRequirements() {
    var mech1 = Mechanism.createDummy("Mech 1", m_scheduler);
    var mech2 = Mechanism.createDummy("Mech 2", m_scheduler);
    var command1 = mech1.run(Coroutine::park).named("Command 1");
    var command2 = mech2.run(Coroutine::park).named("Command 2");
    var group = new ParallelGroup("Group", Set.of(command1, command2), Set.of());
    assertEquals(Set.of(mech1, mech2), group.requirements(), "Requirements were not inherited");
  }

  @Test
  void inheritsPriority() {
    var mech1 = Mechanism.createDummy("Mech 1", m_scheduler);
    var mech2 = Mechanism.createDummy("Mech 2", m_scheduler);
    var command1 = mech1.run(Coroutine::park).withPriority(100).named("Command 1");
    var command2 = mech2.run(Coroutine::park).withPriority(200).named("Command 2");
    var group = new ParallelGroup("Group", Set.of(command1, command2), Set.of());
    assertEquals(200, group.priority(), "Priority was not inherited");
  }
}
