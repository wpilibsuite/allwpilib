// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.digitalcommunication;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;
import org.junit.jupiter.params.provider.ValueSource;
import org.wpilib.hardware.hal.AllianceStationID;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.simulation.DIOSim;
import org.wpilib.simulation.DriverStationSim;
import org.wpilib.simulation.SimHooks;

@ResourceLock("timing")
class DigitalCommunicationTest {
  private Robot robot;
  private Thread thread;
  private final DIOSim allianceOutput = new DIOSim(Robot.kAlliancePort);
  private final DIOSim enabledOutput = new DIOSim(Robot.kEnabledPort);
  private final DIOSim autonomousOutput = new DIOSim(Robot.kAutonomousPort);
  private final DIOSim alertOutput = new DIOSim(Robot.kAlertPort);

  @BeforeEach
  void startThread() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    SimHooks.setProgramStarted(false);
    DriverStationSim.resetData();
    robot = new Robot();
    thread = new Thread(robot::startCompetition);
    thread.start();
    SimHooks.waitForProgramStart();
  }

  @AfterEach
  void stopThread() {
    robot.endCompetition();
    try {
      thread.interrupt();
      thread.join();
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    robot.close();
    allianceOutput.resetData();
    enabledOutput.resetData();
    autonomousOutput.resetData();
    alertOutput.resetData();
  }

  @EnumSource(AllianceStationID.class)
  @ParameterizedTest(name = "alliance[{index}]: {0}")
  void allianceTest(AllianceStationID alliance) {
    DriverStationSim.setAllianceStationId(alliance);
    DriverStationSim.notifyNewData();

    assertTrue(allianceOutput.getInitialized());
    assertFalse(allianceOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(alliance.name().startsWith("RED"), allianceOutput.getValue());
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest(name = "enabled[{index}]: {0}")
  void enabledTest(boolean enabled) {
    DriverStationSim.setEnabled(enabled);
    DriverStationSim.notifyNewData();

    assertTrue(enabledOutput.getInitialized());
    assertFalse(enabledOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(enabled, enabledOutput.getValue());
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest(name = "autonomous[{index}]: {0}")
  void autonomousTest(boolean autonomous) {
    DriverStationSim.setRobotMode(autonomous ? RobotMode.AUTONOMOUS : RobotMode.TELEOPERATED);
    DriverStationSim.notifyNewData();

    assertTrue(autonomousOutput.getInitialized());
    assertFalse(autonomousOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(autonomous, autonomousOutput.getValue());
  }

  @ValueSource(doubles = {45.0, 27.0, 23.0})
  @ParameterizedTest(name = "alert[{index}]: {0}s")
  void alertTest(double matchTime) {
    DriverStationSim.setMatchTime(matchTime);
    DriverStationSim.notifyNewData();

    assertTrue(alertOutput.getInitialized());
    assertFalse(alertOutput.getIsInput());

    SimHooks.stepTiming(0.02);

    assertEquals(matchTime <= 30 && matchTime >= 25, alertOutput.getValue());
  }
}
