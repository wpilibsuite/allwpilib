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
import org.wpilib.backend.NetworkTablesTunableBackend;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.tunable.MockTunableBackend;
import org.wpilib.tunable.TunableConfig;
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
  void runningTunableAlwaysGetsSchedulerState() {
    var tunable = m_backend.getTunable("/command/running");
    assertEquals(TunableConfig.Polling.ALWAYS_GET, tunable.getConfig().getPolling());
  }

  @Test
  void nameTunablePublishesCommandName() {
    m_command.setName("Renamed Command");
    TunableRegistry.update();

    var tunable = m_backend.getTunable("/command/name");
    assertFalse(tunable.getConfig().isMutable());
    assertEquals(TunableConfig.Polling.GET_ON_CHANGE, tunable.getConfig().getPolling());
    assertEquals("Renamed Command", m_backend.getValue("/command/name", String.class));
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

  @Test
  void networkTablesLifecycleMatchesGlassRunCancel() {
    TunableRegistry.reset();
    try (NetworkTableInstance inst = NetworkTableInstance.create()) {
      TunableRegistry.registerBackend("", new NetworkTablesTunableBackend(inst, "/Tunables"));

      AtomicInteger initializeCount = new AtomicInteger();
      AtomicInteger executeCount = new AtomicInteger();
      AtomicInteger interruptedEndCount = new AtomicInteger();
      AtomicInteger finishedEndCount = new AtomicInteger();
      Command command =
          new FunctionalCommand(
              initializeCount::incrementAndGet,
              executeCount::incrementAndGet,
              interrupted -> {
                if (interrupted) {
                  interruptedEndCount.incrementAndGet();
                } else {
                  finishedEndCount.incrementAndGet();
                }
              },
              () -> false);
      command.setName("Glass Label");
      Tunables.publish("command", command);

      var name = inst.getStringTopic("/Tunables/command/name").subscribe("");
      assertEquals("false", inst.getTopic("/Tunables/command/name").getProperty("mutable"));
      assertEquals("Glass Label", name.get());

      var running = inst.getBooleanTopic("/Tunables/command/running").getEntry(false);
      assertEquals("true", inst.getTopic("/Tunables/command/running").getProperty("mutable"));
      assertFalse(running.get());
      assertFalse(command.isScheduled());

      running.set(true);
      inst.flush();
      TunableRegistry.update();

      assertTrue(command.isScheduled());
      assertTrue(running.get());
      assertEquals(1, initializeCount.get());
      assertEquals(0, executeCount.get());
      assertEquals(0, interruptedEndCount.get());
      assertEquals(0, finishedEndCount.get());

      CommandScheduler.getInstance().run();
      assertEquals(1, executeCount.get());
      TunableRegistry.update();
      assertTrue(running.get());

      command.setName("Updated Label");
      TunableRegistry.update();
      assertEquals("Updated Label", name.get());

      running.set(false);
      inst.flush();
      TunableRegistry.update();

      assertFalse(command.isScheduled());
      assertFalse(running.get());
      assertEquals(1, initializeCount.get());
      assertEquals(1, executeCount.get());
      assertEquals(1, interruptedEndCount.get());
      assertEquals(0, finishedEndCount.get());

      CommandScheduler.getInstance().run();
      assertEquals(1, executeCount.get());
      TunableRegistry.update();
      assertFalse(running.get());
    } finally {
      TunableRegistry.reset();
    }
  }

  @AfterEach
  void tearDown() {
    TunableRegistry.reset();
  }
}
