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
    drive = new Drive();
    sim = new DriveSim();
  }

  @AfterEach
  void tearDown() {
    drive.close();
    CommandScheduler.getInstance().cancelAll();
  }

  Drive drive; // real subsystem
  DriveSim sim; // simulation controller

  @Test
  void driveDistanceTest() {
    Command command =
        drive.driveDistanceCommand(AutoConstants.kDriveDistanceMeters, AutoConstants.kDriveSpeed);

    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    command.schedule();

    for (int i = 0; i < 150; i++) {
      CommandScheduler.getInstance().run();
      sim.simulationPeriodic();
    }
    assertEquals(AutoConstants.kDriveDistanceMeters, drive.getLeftEncoder(), 0.3);
    assertEquals(AutoConstants.kDriveDistanceMeters, drive.getRightEncoder(), 0.3);
  }
}
