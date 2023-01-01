// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.DriveSim;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class DriveTest {
  @BeforeEach
  void setup() {
    m_drive = new Drive();
    m_sim = new DriveSim();
  }

  @AfterEach
  void tearDown() {
    m_drive.close();
    CommandScheduler.getInstance().cancelAll();
  }

  Drive m_drive; // real subsystem
  DriveSim m_sim; // simulation controller

  @Test
  void driveDistanceTest() {
    Command command =
        m_drive.driveDistanceCommand(AutoConstants.kDriveDistanceMeters, AutoConstants.kDriveSpeed);

    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    command.schedule();

    for (int i = 0; i < 150; i++) {
      CommandScheduler.getInstance().run();
      m_sim.simulationPeriodic();
    }
    assertEquals(AutoConstants.kDriveDistanceMeters, m_drive.getLeftEncoder(), 0.3);
    assertEquals(AutoConstants.kDriveDistanceMeters, m_drive.getRightEncoder(), 0.3);
  }
}
