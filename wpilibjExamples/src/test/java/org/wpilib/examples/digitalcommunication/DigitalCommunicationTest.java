// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.examples.digitalcommunication;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.AllianceStationID;
import edu.wpi.first.hal.HAL;
import org.wpilib.simulation.DIOSim;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.SimHooks;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;
import org.junit.jupiter.params.provider.ValueSource;

@ResourceLock("timing")
class DigitalCommunicationTest {
  private Robot m_robot;
  private Thread m_thread;
  private final DIOSim m_allianceOutput = new DIOSim(Robot.kAlliancePort);
  private final DIOSim m_enabledOutput = new DIOSim(Robot.kEnabledPort);
  private final DIOSim m_autonomousOutput = new DIOSim(Robot.kAutonomousPort);
  private final DIOSim m_alertOutput = new DIOSim(Robot.kAlertPort);

  @BeforeEach
  void startThread() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    DriverStationSim.resetData();
    m_robot = new Robot();
    m_thread = new Thread(m_robot::startCompetition);
    m_thread.start();
    SimHooks.stepTiming(0.0); // Wait for Notifiers
  }

  @AfterEach
  void stopThread() {
    m_robot.endCompetition();
    try {
      m_thread.interrupt();
      m_thread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    m_robot.close();
    m_allianceOutput.resetData();
    m_enabledOutput.resetData();
    m_autonomousOutput.resetData();
    m_alertOutput.resetData();
  }

  @EnumSource(AllianceStationID.class)
  @ParameterizedTest(name = "alliance[{index}]: {0}")
  void allianceTest(AllianceStationID alliance) {
    DriverStationSim.setAllianceStationId(alliance);
    DriverStationSim.notifyNewData();

    assertTrue(m_allianceOutput.getInitialized());
    assertFalse(m_allianceOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(alliance.name().startsWith("Red"), m_allianceOutput.getValue());
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest(name = "enabled[{index}]: {0}")
  void enabledTest(boolean enabled) {
    DriverStationSim.setEnabled(enabled);
    DriverStationSim.notifyNewData();

    assertTrue(m_enabledOutput.getInitialized());
    assertFalse(m_enabledOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(enabled, m_enabledOutput.getValue());
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest(name = "autonomous[{index}]: {0}")
  void autonomousTest(boolean autonomous) {
    DriverStationSim.setAutonomous(autonomous);
    DriverStationSim.notifyNewData();

    assertTrue(m_autonomousOutput.getInitialized());
    assertFalse(m_autonomousOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(autonomous, m_autonomousOutput.getValue());
  }

  @ValueSource(doubles = {45.0, 27.0, 23.0})
  @ParameterizedTest(name = "alert[{index}]: {0}s")
  void alertTest(double matchTime) {
    DriverStationSim.setMatchTime(matchTime);
    DriverStationSim.notifyNewData();

    assertTrue(m_alertOutput.getInitialized());
    assertFalse(m_alertOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(matchTime <= 30 && matchTime >= 25, m_alertOutput.getValue());
  }
}
