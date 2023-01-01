package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems; // Copyright (c) FIRST and
// other WPILib
// contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

import static org.junit.jupiter.api.Assertions.*;

import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.ShooterSim;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

// @TestInstance(Lifecycle.PER_CLASS)
class ShooterTest {
  @BeforeEach
  void setup() {
    shooter = new Shooter();
    sim = new ShooterSim();
  }

  @AfterEach
  void teardown() {
    shooter.close();
    sim.reset();
    CommandScheduler.getInstance().cancelAll();
  }

  Shooter shooter; // real subsystem
  ShooterSim sim; // simulation controller

  @Test
  void idleCommandTest() {
    // Check default command registered properly
    Command defaultCommand = shooter.getDefaultCommand();
    assertNotEquals(null, defaultCommand);
    assertEquals("Idle", defaultCommand.getName());
    assertTrue(defaultCommand.runsWhenDisabled());

    // Check default command functionality
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    var command = shooter.getCurrentCommand();
    assertNotEquals(null, command);
    assertEquals("Idle", command.getName());
    assertEquals(0.0, sim.getShooterMotor());
    assertEquals(0.0, sim.getFeederMotor());
  }

  @Test
  void shootCommandTest() {
    Command command = shooter.shootCommand(ShooterConstants.kShooterTargetRPS);
    assertEquals("Shoot", command.getName());

    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    command.schedule();

    assertEquals(0.0, sim.getFeederMotor());

    for (int i = 0; i < 350; i++) {
      CommandScheduler.getInstance().run();
      sim.simulationPeriodic();
      assertNotEquals(0.0, sim.getShooterMotor());
    }
    assertEquals(
        ShooterConstants.kShooterTargetRPS,
        shooter.getShooterVelocity(),
        ShooterConstants.kShooterToleranceRPS);
    assertEquals(ShooterConstants.kFeederSpeed, sim.getFeederMotor());
  }
}
