// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

class CommandTunableButtonTest extends CommandTestBase {
  private MockTunableBackend m_backend;
  private AtomicInteger m_schedule;
  private AtomicInteger m_cancel;
  private Command m_command;

  @BeforeEach
  void setUp() {
    m_backend = new MockTunableBackend();
    TunableRegistry.registerBackend("", m_backend);
    m_schedule = new AtomicInteger();
    m_cancel = new AtomicInteger();
    m_command = Commands.startEnd(m_schedule::incrementAndGet, m_cancel::incrementAndGet);
    Tunables.publish("command", m_command);
  }

  @Test
  void trueAndNotScheduledSchedules() {
    // Not scheduled and true -> scheduled
    CommandScheduler.getInstance().run();
    assertFalse(m_command.isScheduled());
    assertEquals(0, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_backend.setBoolean("/command/running", true);
    TunableRegistry.update();
    CommandScheduler.getInstance().run();
    assertTrue(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(0, m_cancel.get());
  }

  @Test
  void trueAndScheduledNoOp() {
    // Scheduled and true -> no-op
    CommandScheduler.getInstance().schedule(m_command);
    CommandScheduler.getInstance().run();
    assertTrue(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_backend.setBoolean("/command/running", true);
    TunableRegistry.update();
    CommandScheduler.getInstance().run();
    assertTrue(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(0, m_cancel.get());
  }

  @Test
  void falseAndNotScheduledNoOp() {
    // Not scheduled and false -> no-op
    CommandScheduler.getInstance().run();
    assertFalse(m_command.isScheduled());
    assertEquals(0, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_backend.setBoolean("/command/running", false);
    TunableRegistry.update();
    CommandScheduler.getInstance().run();
    assertFalse(m_command.isScheduled());
    assertEquals(0, m_schedule.get());
    assertEquals(0, m_cancel.get());
  }

  @Test
  void falseAndScheduledCancel() {
    // Scheduled and false -> cancel
    CommandScheduler.getInstance().schedule(m_command);
    CommandScheduler.getInstance().run();
    assertTrue(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_backend.setBoolean("/command/running", false);
    TunableRegistry.update();
    CommandScheduler.getInstance().run();
    assertFalse(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(1, m_cancel.get());
  }

  @AfterEach
  void tearDown() {
    TunableRegistry.reset();
  }
}
