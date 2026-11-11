// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

class CommandComplexTunableTest extends CommandTestBase {
  private MockTunableBackend m_dashboard;
  private DummyMechanism m_mech;
  private Command m_cmd;

  @BeforeEach
  void initTunables() {
    m_dashboard = new MockTunableBackend();
    m_mech = new DummyMechanism("Mech", m_scheduler);
    m_cmd = Command.requiring(m_mech).executing(Coroutine::park).named("Command");

    TunableRegistry.reset();
    TunableRegistry.registerBackend("", m_dashboard);
  }

  @AfterEach
  void resetTunables() {
    TunableRegistry.reset();
  }

  @Test
  void publishCommandNameAndRunningValue() {
    // Check that command gets published with correct name & running flag set false.
    Tunables.publish("cmd", m_cmd);
    assertEquals("Command", m_dashboard.getValue("/cmd/name", String.class));
    assertFalse(m_dashboard.getBoolean("/cmd/running"));
    assertFalse(m_scheduler.isScheduledOrRunning(m_cmd));
  }

  @Test
  void toggleRunningSchedulesAndCancelsCommand() {
    Tunables.publish("cmd", m_cmd);

    // Check that command runs when value is true
    m_dashboard.setBoolean("/cmd/running", true);
    TunableRegistry.update();
    assertTrue(m_scheduler.isScheduledOrRunning(m_cmd));

    // Check that command does not run when value is false
    m_dashboard.setBoolean("/cmd/running", false);
    TunableRegistry.update();
    assertFalse(m_scheduler.isScheduledOrRunning(m_cmd));
  }

  @Test
  void runningStateReflectsScheduled() {
    Tunables.publish("cmd", m_cmd);

    // Check that scheduling/cancelling a command independently of dashboard updates the running
    // value
    m_scheduler.schedule(m_cmd);
    TunableRegistry.update();
    assertTrue(m_dashboard.getBoolean("/cmd/running"));

    m_scheduler.cancel(m_cmd);
    TunableRegistry.update();
    assertFalse(m_dashboard.getBoolean("/cmd/running"));
  }

  @Test
  void tunableTypeIsCommand() {
    // Check that the tunable type is "Command"
    assertEquals("Command", m_cmd.getTunableType());
  }
}
