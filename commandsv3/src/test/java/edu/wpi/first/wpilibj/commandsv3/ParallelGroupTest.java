package edu.wpi.first.wpilibj.commandsv3;

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

    // First call to run() should schedule the commands
    scheduler.run();
    assertTrue(scheduler.isRunning(parallel));
    assertTrue(scheduler.isScheduled(c1));
    assertTrue(scheduler.isScheduled(c2));

    // Next call to run() should start them
    for (int i = 0; i < 5; i++) {
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
    // one final run() should unschedule the c2 command and then the parallel command
    scheduler.run();
    scheduler.run();
    assertFalse(scheduler.isRunning(parallel));
    assertFalse(scheduler.isRunning(c1));
    assertFalse(scheduler.isRunning(c2));

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
    assertTrue(scheduler.isScheduled(c1));
    assertTrue(scheduler.isScheduled(c2));

    for (int i = 0; i < 5; i++) {
      scheduler.run();
      assertTrue(scheduler.isRunning(c1));
      assertTrue(scheduler.isRunning(c2));
      assertEquals(i, c1Count.get());
      assertEquals(i, c2Count.get());
    }
    scheduler.run(); // complete c1
    scheduler.run(); // complete parallel and cleanup
    assertFalse(scheduler.isRunning(race));
    assertFalse(scheduler.isRunning(c1));
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

    var inner = ParallelGroup.all("Inner", command);
    var outer = ParallelGroup.all("Outer", inner);

    // Scheduling: Outer group should be on deck
    scheduler.schedule(outer);
    assertTrue(scheduler.isScheduled(outer));
    assertFalse(scheduler.isScheduledOrRunning(inner));
    assertFalse(scheduler.isScheduledOrRunning(command));

    // First run: Inner group should be scheduled, command shouldn't be yet
    scheduler.run();
    assertTrue(scheduler.isRunning(outer));
    assertTrue(scheduler.isScheduled(inner));
    assertFalse(scheduler.isScheduledOrRunning(command));

    // Second run: Outer and inner should both be running, command should be scheduled
    scheduler.run();
    assertTrue(scheduler.isRunning(outer), "Outer group should be running");
    assertTrue(scheduler.isRunning(inner), "Inner group should be running");
    assertTrue(scheduler.isScheduled(command), "Command should be scheduled");

    // Runs 3 through 8: Outer and inner should both be running while the command runs
    for (int i = 0; i < 5; i++) {
      scheduler.run();
      assertTrue(scheduler.isRunning(outer), "Outer group should be running");
      assertTrue(scheduler.isRunning(inner), "Inner group should be running");
      assertTrue(scheduler.isRunning(command), "Command should be running");
    }

    // Run 9: Command should have completed naturally
    scheduler.run();
    assertTrue(scheduler.isRunning(outer), "Outer group should be running");
    assertTrue(scheduler.isRunning(inner), "Inner group should be running");
    assertFalse(scheduler.isRunning(command), "Command should have completed");

    // Run 10: Having seen the command complete, inner group should exit
    scheduler.run();
    assertTrue(scheduler.isRunning(outer), "Outer group should be running");
    assertFalse(scheduler.isRunning(inner), "Inner group should have completed");
    assertFalse(scheduler.isRunning(command), "Command should have completed");

    // Run 11: Having seen the inner group complete, outer group should now exit
    scheduler.run();
    assertFalse(scheduler.isRunning(outer), "Outer group should be running");
    assertFalse(scheduler.isRunning(inner), "Inner group should have completed");
    assertFalse(scheduler.isRunning(command), "Command should have completed");
  }
}
