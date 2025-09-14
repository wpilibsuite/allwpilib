// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.junit.jupiter.api.Assertions.fail;

import org.junit.jupiter.api.Test;

class SchedulerPriorityLevelTests extends CommandTestBase {
  @Test
  void higherPriorityCancels() {
    final var subsystem = new Mechanism("Subsystem", m_scheduler);

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
    final var subsystem = new Mechanism("Subsystem", m_scheduler);

    final var lower = new PriorityCommand(-1000, subsystem);
    final var higher = new PriorityCommand(+1000, subsystem);

    m_scheduler.schedule(higher);
    m_scheduler.run();
    assertTrue(m_scheduler.isRunning(higher));

    m_scheduler.schedule(lower);
    m_scheduler.run();
    if (!m_scheduler.isRunning(higher)) {
      fail("Higher priority command should still be running");
    }
    if (m_scheduler.isRunning(lower)) {
      fail("Lower priority command should not be running");
    }
  }

  @Test
  void samePriorityCancels() {
    final var subsystem = new Mechanism("Subsystem", m_scheduler);

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
}
