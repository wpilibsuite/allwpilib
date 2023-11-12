// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.CommandTestBase;
import org.junit.jupiter.api.Test;

class GameTriggersTest extends CommandTestBase {
  @Test
  void autonomousTest() {
    DriverStationSim.resetData();
    DriverStationSim.setAutonomous(true);
    DriverStationSim.setTest(false);
    DriverStationSim.setEnabled(true);
    DriverStation.refreshData();
    Trigger auto = GameTriggers.autonomous();
    assertTrue(auto.getAsBoolean());
  }

  @Test
  void teleopTest() {
    DriverStationSim.resetData();
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.setEnabled(true);
    DriverStation.refreshData();
    Trigger teleop = GameTriggers.teleop();
    assertTrue(teleop.getAsBoolean());
  }

  @Test
  void testModeTest() {
    DriverStationSim.resetData();
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(true);
    DriverStationSim.setEnabled(true);
    DriverStation.refreshData();
    Trigger test = GameTriggers.test();
    assertTrue(test.getAsBoolean());
  }

  @Test
  void disabledTest() {
    DriverStationSim.resetData();
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setTest(false);
    DriverStationSim.setEnabled(false);
    DriverStation.refreshData();
    Trigger disabled = GameTriggers.disabled();
    assertTrue(disabled.getAsBoolean());
  }
}
