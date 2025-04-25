// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class SequenceTest {
  private Scheduler scheduler;

  @BeforeEach
  void setup() {
    scheduler = new Scheduler();
  }

  @Test
  void single() {
    var command = Command.noRequirements(Coroutine::yield).named("The Command");

    var sequence = new Sequence("The Sequence", List.of(command));
    scheduler.schedule(sequence);

    // First run - the composed command is scheduled and starts
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertTrue(scheduler.isRunning(command));

    // Second run - the composed command completes
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isRunning(command));

    // Third run - sequence sees the composed command is done and completes
    scheduler.run();
    assertFalse(scheduler.isRunning(sequence));
    assertFalse(scheduler.isRunning(command));
  }

  @Test
  void twoCommands() {
    var c1 = Command.noRequirements(Coroutine::yield).named("C1");
    var c2 = Command.noRequirements(Coroutine::yield).named("C2");

    var sequence = new Sequence("C1 > C2", List.of(c1, c2));
    scheduler.schedule(sequence);

    // First run - c1 is scheduled and starts
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence), "Sequence should be running");
    assertTrue(scheduler.isRunning(c1), "Starting the sequence should start the first command");
    assertFalse(
        scheduler.isScheduledOrRunning(c2),
        "The second command should still be pending completion of the first command");

    // Second run - c1 completes
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isRunning(c1), "First command should have completed");
    assertFalse(
        scheduler.isScheduledOrRunning(c2), "Second command should not start in the same cycle");

    // Third run - c2 is scheduled and starts
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertTrue(scheduler.isRunning(c2), "Second command should have started");

    // Fourth run - c2 completes
    scheduler.run();
    assertTrue(scheduler.isRunning(sequence));
    assertFalse(scheduler.isRunning(c2), "Second command should have completed");

    // Fifth run - sequence completes
    scheduler.run();
    assertFalse(scheduler.isRunning(sequence), "Sequence should have completed");
    assertFalse(scheduler.isRunning(c1), "First command should have stopped");
    assertFalse(scheduler.isRunning(c2), "Second command should have stopped");
  }
}
