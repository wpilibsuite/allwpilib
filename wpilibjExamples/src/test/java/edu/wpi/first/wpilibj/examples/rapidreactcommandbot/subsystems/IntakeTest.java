// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.IntakeConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.IntakeSim;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class IntakeTest {
  @BeforeEach
  void setup() {
    m_intake = new Intake();
    m_sim = new IntakeSim();
  }

  @AfterEach
  void tearDown() {
    m_intake.close();
    m_sim.reset();
    CommandScheduler.getInstance().cancelAll();
  }

  Intake m_intake; // real subsystem
  IntakeSim m_sim; // simulation controller

  @Test
  void intakeCommandTest() {
    Command command = m_intake.intakeCommand();

    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    command.schedule();

    CommandScheduler.getInstance().run();
    m_sim.simulationPeriodic();
    CommandScheduler.getInstance().run();
    m_sim.simulationPeriodic();

    assertEquals(IntakeConstants.kIntakeDutyCycle, m_sim.getMotor(), 1e-6);
    assertTrue(m_sim.isDeployed());
  }

  @Test
  void retractCommandTest() {
    Command command = m_intake.retractCommand();

    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    command.schedule();

    CommandScheduler.getInstance().run();
    m_sim.simulationPeriodic();
    CommandScheduler.getInstance().run();
    m_sim.simulationPeriodic();

    assertEquals(0.0, m_sim.getMotor(), 1e-6);
    assertFalse(m_sim.isDeployed());
  }
}
