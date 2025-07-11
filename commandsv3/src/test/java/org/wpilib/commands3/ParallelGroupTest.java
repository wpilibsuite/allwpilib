// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.*;

import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class ParallelGroupTest {
  private Scheduler scheduler;

  @BeforeEach
  void setup() {
    scheduler = new Scheduler();
  }

  @Test
  void parallelAll() {
    var r1 = new RequireableResource("R1", scheduler);
    var r2 = new RequireableResource("R2", scheduler);

    var c1Count = new AtomicInteger(0);
    var c2Count = new AtomicInteger(0);

    var c1 =
        r1.run(
                (coroutine) -> {
                  for (int i = 0; i < 5; i++) {
                    coroutine.yield();
                    c1Count.incrementAndGet();
                  }
                })
            .named("C1");
    var c2 =
        r2.run(
                (coroutine) -> {
                  for (int i = 0; i < 10; i++) {
                    coroutine.yield();
                    c2Count.incrementAndGet();
                  }
                })
            .named("C2");

    var parallel = new ParallelGroup("Parallel", List.of(c1, c2), List.of(c1, c2));
    scheduler.schedule(parallel);

    // First call to run() should schedule and start the commands
    scheduler.run();
    assertTrue(scheduler.isRunning(parallel));
    assertTrue(scheduler.isRunning(c1));
    assertTrue(scheduler.isRunning(c2));

    // Next call to run() should start them
    for (int i = 1; i < 5; i++) {
      scheduler.run();
      assertTrue(scheduler.isRunning(c1));
      assertTrue(scheduler.isRunning(c2));
      assertEquals(i, c1Count.get());
      assertEquals(i, c2Count.get());
    }
    // c1 should finish after 5 iterations; c2 should continue for another 5
    for (int i = 5; i < 10; i++) {
      scheduler.run();
      assertFalse(scheduler.isRunning(c1));
      assertTrue(scheduler.isRunning(c2));
      assertEquals(5, c1Count.get());
      assertEquals(i, c2Count.get());
    }
    // one final run() should unschedule the c2 command
    scheduler.run();
    assertFalse(scheduler.isRunning(c1));
    assertFalse(scheduler.isRunning(c2));

    // the next run should complete the group
    scheduler.run();
    assertFalse(scheduler.isRunning(parallel));

    // and final counts should be 5 and 10
    assertEquals(5, c1Count.get());
    assertEquals(10, c2Count.get());
  }

  @Test
  void race() {
    var r1 = new RequireableResource("R1", scheduler);
    var r2 = new RequireableResource("R2", scheduler);

    var c1Count = new AtomicInteger(0);
    var c2Count = new AtomicInteger(0);

    var c1 =
        r1.run(
                (coroutine) -> {
                  for (int i = 0; i < 5; i++) {
                    coroutine.yield();
                    c1Count.incrementAndGet();
                  }
                })
            .named("C1");
    var c2 =
        r2.run(
                (coroutine) -> {
                  for (int i = 0; i < 10; i++) {
                    coroutine.yield();
                    c2Count.incrementAndGet();
                  }
                })
            .named("C2");

    var race = new ParallelGroup("Race", List.of(c1, c2), List.of());
    scheduler.schedule(race);

    // First call to run() should schedule the commands
    scheduler.run();
    assertTrue(scheduler.isRunning(race));
    assertTrue(scheduler.isRunning(c1));
    assertTrue(scheduler.isRunning(c2));

    for (int i = 1; i < 5; i++) {
      scheduler.run();
      assertTrue(scheduler.isRunning(c1));
      assertTrue(scheduler.isRunning(c2));
      assertEquals(i, c1Count.get());
      assertEquals(i, c2Count.get());
    }
    scheduler.run(); // complete c1
    assertTrue(scheduler.isRunning(race));
    assertFalse(scheduler.isRunning(c1));
    assertTrue(scheduler.isRunning(c2));

    scheduler.run(); // complete parallel and cleanup
    assertFalse(scheduler.isRunning(race));
    assertFalse(scheduler.isRunning(c2));

    // and final counts should be 5 and 5
    assertEquals(5, c1Count.get());
    assertEquals(5, c2Count.get());
  }

  @Test
  void nested() {
    var resource = new RequireableResource("Resource", scheduler);

    var count = new AtomicInteger(0);

    var command =
        resource
            .run(
                (coroutine) -> {
                  for (int i = 0; i < 5; i++) {
                    coroutine.yield();
                    count.incrementAndGet();
                  }
                })
            .named("Command");

    var inner = ParallelGroup.all(command).named("Inner");
    var outer = ParallelGroup.all(inner).named("Outer");

    // Scheduling: Outer group should be on deck
    scheduler.schedule(outer);
    assertTrue(scheduler.isScheduled(outer));
    assertFalse(scheduler.isScheduledOrRunning(inner));
    assertFalse(scheduler.isScheduledOrRunning(command));

    // First run: Inner group and command should both be scheduled and running
    scheduler.run();
    assertTrue(scheduler.isRunning(outer), "Outer group should be running");
    assertTrue(scheduler.isRunning(inner), "Inner group should be running");
    assertTrue(scheduler.isRunning(command), "Command should be running");
    assertEquals(0, count.get());

    // Runs 2 through 5: Outer and inner should both be running while the command runs
    for (int i = 1; i < 5; i++) {
      scheduler.run();
      assertTrue(scheduler.isRunning(outer), "Outer group should be running");
      assertTrue(scheduler.isRunning(inner), "Inner group should be running");
      assertTrue(scheduler.isRunning(command), "Command should be running (" + i + ")");
      assertEquals(i, count.get());
    }

    // Run 6: Command should have completed naturally
    scheduler.run();
    assertTrue(scheduler.isRunning(outer), "Outer group should be running");
    assertTrue(scheduler.isRunning(inner), "Inner group should be running");
    assertFalse(scheduler.isRunning(command), "Command should have completed");

    // Run 7: Having seen the command complete, inner group should exit
    scheduler.run();
    assertTrue(scheduler.isRunning(outer), "Outer group should be running");
    assertFalse(scheduler.isRunning(inner), "Inner group should have completed");
    assertFalse(scheduler.isRunning(command), "Command should have completed");

    // Run 8: Having seen the inner group complete, outer group should now exit
    scheduler.run();
    assertFalse(scheduler.isRunning(outer), "Outer group should be running");
    assertFalse(scheduler.isRunning(inner), "Inner group should have completed");
    assertFalse(scheduler.isRunning(command), "Command should have completed");
  }

  @Test
  void automaticNameRace() {
    var a = Command.noRequirements((coroutine) -> {}).named("A");
    var b = Command.noRequirements((coroutine) -> {}).named("B");
    var c = Command.noRequirements((coroutine) -> {}).named("C");

    var group = ParallelGroup.builder().optional(a, b, c).withAutomaticName();
    assertEquals("(A | B | C)", group.name());
  }

  @Test
  void automaticNameAll() {
    var a = Command.noRequirements((coroutine) -> {}).named("A");
    var b = Command.noRequirements((coroutine) -> {}).named("B");
    var c = Command.noRequirements((coroutine) -> {}).named("C");

    var group = ParallelGroup.builder().requiring(a, b, c).withAutomaticName();
    assertEquals("(A & B & C)", group.name());
  }

  @Test
  void automaticNameDeadline() {
    var a = Command.noRequirements((coroutine) -> {}).named("A");
    var b = Command.noRequirements((coroutine) -> {}).named("B");
    var c = Command.noRequirements((coroutine) -> {}).named("C");

    var group = ParallelGroup.builder().requiring(a).optional(b, c).withAutomaticName();
    assertEquals("[(A) * (B | C)]", group.name());
  }
}
