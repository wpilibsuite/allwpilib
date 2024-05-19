package edu.wpi.first.wpilibj3.command.async;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class SequenceTest {
  private AsyncScheduler scheduler;

  @BeforeEach
  void setup() {
    scheduler = new AsyncScheduler();
  }

  @Test
  void single() {
    var command = AsyncCommand.noHardware(Coroutine::yield).named("The Command");

    var sequence = new Sequence("The Sequence", List.of(command));
    scheduler.schedule(sequence);

    // First run - the composed command is scheduled
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertTrue(scheduler.isScheduled(command));

    // Second run - starts running the composed command
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertTrue(scheduler.isRunning(command));

    // Third run - sequence continues and composed command completes
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isRunning(command));

    // Fourth run - sequence sees the composed command is done and completes
    scheduler.run();
    assertFalse(scheduler.isRunning(sequence));
    assertFalse(scheduler.isRunning(command));
  }

  @Test
  void twoCommands() {
    var c1 = AsyncCommand.noHardware(Coroutine::yield).named("C1");
    var c2 = AsyncCommand.noHardware(Coroutine::yield).named("C2");

    var sequence = new Sequence("C1 > C2", List.of(c1, c2));
    scheduler.schedule(sequence);

    // First run - c1 is scheduled
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertTrue(scheduler.isScheduled(c1));
    assertFalse(scheduler.isScheduledOrRunning(c2));

    // Second run - c1 starts running
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertTrue(scheduler.isRunning(c1));
    assertFalse(scheduler.isScheduledOrRunning(c2));

    // Third run - c1 stops
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isRunning(c1));
    assertFalse(scheduler.isScheduledOrRunning(c2));

    // Fourth run - c2 starts
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isScheduledOrRunning(c1));
    assertTrue(scheduler.isScheduled(c2));

    // Fifth run - c2 starts running
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isScheduledOrRunning(c1));
    assertTrue(scheduler.isRunning(c2));

    // Sixth run - c2 completes
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isScheduledOrRunning(c1));
    assertFalse(scheduler.isScheduledOrRunning(c2));

    // Seventh run - sequence completes
    scheduler.run();
    assertFalse(scheduler.isRunning(sequence));
    assertFalse(scheduler.isScheduledOrRunning(c1));
    assertFalse(scheduler.isScheduledOrRunning(c2));
  }
}