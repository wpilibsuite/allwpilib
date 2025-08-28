// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.NetworkTableInstance;
import org.wpilib.smartdashboard.SmartDashboard;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class CommandSendableButtonTest extends CommandTestBase {
  private NetworkTableInstance m_inst;
  private AtomicInteger m_schedule;
  private AtomicInteger m_cancel;
  private BooleanPublisher m_publish;
  private Command m_command;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    SmartDashboard.setNetworkTableInstance(m_inst);
    m_schedule = new AtomicInteger();
    m_cancel = new AtomicInteger();
    m_command = Commands.startEnd(m_schedule::incrementAndGet, m_cancel::incrementAndGet);
    m_publish = m_inst.getBooleanTopic("/SmartDashboard/command/running").publish();
    SmartDashboard.putData("command", m_command);
    SmartDashboard.updateValues();
  }

  @Test
  void trueAndNotScheduledSchedules() {
    // Not scheduled and true -> scheduled
    CommandScheduler.getInstance().run();
    SmartDashboard.updateValues();
    assertFalse(m_command.isScheduled());
    assertEquals(0, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_publish.set(true);
    SmartDashboard.updateValues();
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
    SmartDashboard.updateValues();
    assertTrue(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_publish.set(true);
    SmartDashboard.updateValues();
    CommandScheduler.getInstance().run();
    assertTrue(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(0, m_cancel.get());
  }

  @Test
  void falseAndNotScheduledNoOp() {
    // Not scheduled and false -> no-op
    CommandScheduler.getInstance().run();
    SmartDashboard.updateValues();
    assertFalse(m_command.isScheduled());
    assertEquals(0, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_publish.set(false);
    SmartDashboard.updateValues();
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
    SmartDashboard.updateValues();
    assertTrue(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(0, m_cancel.get());

    m_publish.set(false);
    SmartDashboard.updateValues();
    CommandScheduler.getInstance().run();
    assertFalse(m_command.isScheduled());
    assertEquals(1, m_schedule.get());
    assertEquals(1, m_cancel.get());
  }

  @AfterEach
  void tearDown() {
    m_publish.close();
    m_inst.close();
    SmartDashboard.setNetworkTableInstance(NetworkTableInstance.getDefault());
  }
}
