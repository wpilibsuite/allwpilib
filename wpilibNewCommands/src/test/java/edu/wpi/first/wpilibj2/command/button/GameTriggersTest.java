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
    Trigger auto = GameTriggers.autonomous();
    DriverStationSim.setAutonomous(true);
    DriverStation.refreshData();
    assertTrue(auto.getAsBoolean());
  }
}
