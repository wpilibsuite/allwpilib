package edu.wpi.first.wpilibj3.command.async;

import static org.junit.jupiter.api.Assertions.*;

import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class ParallelGroupTest {
  private AsyncScheduler scheduler;

  @BeforeEach
  void setup() {
    Logger.clear();

    // Any command that calls AsyncCommand.park() or AsyncCommand.yield() uses the default
    // scheduler instance. Notably, this includes commands with timeouts, wait commands, and
    // the default idle commands
    scheduler = new AsyncScheduler();
    AsyncScheduler.setDefaultScheduler(scheduler);
  }

  @AfterEach
  void printLogs() {
    System.out.println(Logger.formattedLogTable());
    Logger.clear();
  }

  @Test
  void parallelAll() {
    var r1 = new HardwareResource("R1", scheduler);
    var r2 = new HardwareResource("R2", scheduler);

    var c1Count = new AtomicInteger(0);
    var c2Count = new AtomicInteger(0);

    var c1 = r1.run(() -> {
      for (int i = 0; i < 5; i++) {
        scheduler.yield();
        c1Count.incrementAndGet();
      }
    }).named("C1");
    var c2 = r2.run(() -> {
      for (int i = 0; i < 10; i++) {
        scheduler.yield();
        c2Count.incrementAndGet();
      }
    }).named("C2");

    var parallel = new ParallelGroup("Parallel", scheduler, List.of(c1, c2), List.of(c1, c2));
    scheduler.schedule(parallel);

    // First call to run() should schedule the commands
    scheduler.run();
    assertTrue(scheduler.isRunning(parallel));
    assertFalse(scheduler.isRunning(c1));
    assertFalse(scheduler.isRunning(c2));

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
    var r1 = new HardwareResource("R1", scheduler);
    var r2 = new HardwareResource("R2", scheduler);

    var c1Count = new AtomicInteger(0);
    var c2Count = new AtomicInteger(0);

    var c1 = r1.run(() -> {
      for (int i = 0; i < 5; i++) {
        scheduler.yield();
        c1Count.incrementAndGet();
      }
    }).named("C1");
    var c2 = r2.run(() -> {
      for (int i = 0; i < 10; i++) {
        scheduler.yield();
        c2Count.incrementAndGet();
      }
    }).named("C2");

    var race = new ParallelGroup("Race", scheduler, List.of(c1, c2), List.of());
    scheduler.schedule(race);

    // First call to run() should schedule the commands
    scheduler.run();
    assertTrue(scheduler.isRunning(race));
    assertFalse(scheduler.isRunning(c1));
    assertFalse(scheduler.isRunning(c2));

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
}
