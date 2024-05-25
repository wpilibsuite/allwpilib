// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.ShooterConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.ShooterSim;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class ShooterTest {
  @BeforeEach
  void setup() {
    m_shooter = new Shooter();
    m_sim = new ShooterSim();
  }

  @AfterEach
  void teardown() {
    m_shooter.close();
    m_sim.reset();
    CommandScheduler.getInstance().cancelAll();
  }

  Shooter m_shooter; // real subsystem
  ShooterSim m_sim; // simulation controller

  @Test
  void idleCommandTest() {
    // Check default command registered properly
    Command defaultCommand = m_shooter.getDefaultCommand();
    assertNotEquals(null, defaultCommand);
    assertEquals("Idle", defaultCommand.getName());
    assertTrue(defaultCommand.runsWhenDisabled());

    // Check default command functionality
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    CommandScheduler.getInstance().run();
    var command = m_shooter.getCurrentCommand();
    assertNotEquals(null, command);
    assertEquals("Idle", command.getName());
    assertEquals(0.0, m_sim.getShooterMotor());
    assertEquals(0.0, m_sim.getFeederMotor());
  }

  @Test
  void shootCommandTest() {
    Command command = m_shooter.shootCommand(ShooterConstants.kShooterTargetRPS);
    assertEquals("Shoot", command.getName());

    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    command.schedule();

    assertEquals(0.0, m_sim.getFeederMotor());

    for (int i = 0; i < 350; i++) {
      CommandScheduler.getInstance().run();
      m_sim.simulationPeriodic();
      assertNotEquals(0.0, m_sim.getShooterMotor());
    }
    assertEquals(
        ShooterConstants.kShooterTargetRPS,
        m_shooter.getShooterVelocity(),
        ShooterConstants.kShooterToleranceRPS);
    assertEquals(ShooterConstants.kFeederSpeed, m_sim.getFeederMotor());
  }
}
