// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.RobotMode;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.CommandTestBase;
import org.junit.jupiter.api.Test;

class RobotModeTriggersTest extends CommandTestBase {
  @Test
  void autonomousTest() {
    DriverStationSim.resetData();
    DriverStationSim.setRobotMode(RobotMode.AUTONOMOUS);
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    Trigger auto = RobotModeTriggers.autonomous();
    assertTrue(auto.getAsBoolean());
  }

  @Test
  void teleopTest() {
    DriverStationSim.resetData();
    DriverStationSim.setRobotMode(RobotMode.TELEOPERATED);
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    Trigger teleop = RobotModeTriggers.teleop();
    assertTrue(teleop.getAsBoolean());
  }

  @Test
  void testModeTest() {
    DriverStationSim.resetData();
    DriverStationSim.setRobotMode(RobotMode.TEST);
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();
    Trigger test = RobotModeTriggers.test();
    assertTrue(test.getAsBoolean());
  }

  @Test
  void disabledTest() {
    DriverStationSim.resetData();
    DriverStationSim.setEnabled(false);
    DriverStationSim.notifyNewData();
    Trigger disabled = RobotModeTriggers.disabled();
    assertTrue(disabled.getAsBoolean());
  }
}
