// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import org.junit.jupiter.api.Test;

class SequentialGroupTest extends CommandTestBase {
  @Test
  void single() {
    var command = Command.noRequirements().executing(Coroutine::yield).named("The Command");

    var sequence = new SequentialGroup("The Sequence", List.of(command));
    m_scheduler.schedule(sequence);

    // First run - the composed command starts and yields; sequence yields
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(sequence));
    assertTrue(m_scheduler.isRunning(command));

    // Second run - the composed command completes; sequence sees its completion and exits
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(sequence));
    assertFalse(m_scheduler.isRunning(command));
  }

  @Test
  void twoCommands() {
    var c1 = Command.noRequirements().executing(Coroutine::yield).named("C1");
    var c2 = Command.noRequirements().executing(Coroutine::yield).named("C2");

    var sequence = new SequentialGroup("C1 > C2", List.of(c1, c2));
    m_scheduler.schedule(sequence);

    // First run - c1 is scheduled and starts
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(sequence), "Sequence should be running");
    assertTrue(m_scheduler.isRunning(c1), "Starting the sequence should start the first command");
    assertFalse(
        m_scheduler.isScheduledOrRunning(c2),
        "The second command should still be pending completion of the first command");

    // Second run - c1 completes, sequence sees it finish, schedules c2
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(sequence));
    assertFalse(m_scheduler.isRunning(c1), "First command should have completed");
    assertTrue(
        m_scheduler.isScheduledOrRunning(c2), "Second command should not start in the same cycle");

    // Third run - c2 completes, sequence sees it finish, exits
    m_scheduler.run();
    assertFalse(m_scheduler.isRunning(sequence));
    assertFalse(m_scheduler.isRunning(c2), "Second command should have started");
  }
}
