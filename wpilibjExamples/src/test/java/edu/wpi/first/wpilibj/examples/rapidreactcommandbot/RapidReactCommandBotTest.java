// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.AutoConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.IntakeConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.OIConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.StorageConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.DriveSim;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.IntakeSim;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.ShooterSim;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.StorageSim;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import edu.wpi.first.wpilibj.simulation.XboxControllerSim;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

@ResourceLock("timing")
class RapidReactCommandBotTest {
  private Robot m_robot;
  private Thread m_thread;

  @SuppressWarnings("PMD.SingularField")
  private ShooterSim m_shooterSim;
  private StorageSim m_storageSim;
  private DriveSim m_driveSim;
  private IntakeSim m_intakeSim;
  private XboxControllerSim m_driverController;

  @BeforeEach
  void startThread() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    DriverStationSim.resetData();

    m_robot = new Robot();
    m_storageSim = new StorageSim();
    m_driveSim = new DriveSim();
    m_intakeSim = new IntakeSim();
    m_shooterSim = new ShooterSim();
    m_driverController = new XboxControllerSim(OIConstants.kDriverControllerPort);
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
  }

  @Test
  void autoTest() {
    // auto init
    DriverStationSim.setAutonomous(true);
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    SimHooks.stepTiming(1);

    assertEquals(AutoConstants.kDriveSpeed, m_driveSim.getLeftDutyCycle(), 1e-6);
    assertEquals(AutoConstants.kDriveSpeed, m_driveSim.getRightDutyCycle(), 1e-6);

    // Timeout passed
    SimHooks.stepTiming(2.1);

    assertEquals(AutoConstants.kDriveDistanceMeters, m_driveSim.getRightDistance(), 0.2);
    assertEquals(AutoConstants.kDriveDistanceMeters, m_driveSim.getRightDistance(), 0.2);

    assertEquals(0.0, m_driveSim.getLeftDutyCycle(), 1e-6);
    assertEquals(0.0, m_driveSim.getRightDutyCycle(), 1e-6);
  }

  @Test
  void storageFullTriggersTest() {
    DriverStationSim.setAutonomous(false);
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    {
      // When empty, storage should run.
      m_storageSim.setIsFull(false);
      SimHooks.stepTiming(60);
      assertEquals(StorageConstants.kStorageDutyCycle, m_storageSim.getMotor(), 1e-6);
    }

    {
      // Press the 'X' button
      m_driverController.setXButton(true);
      DriverStationSim.notifyNewData();
      SimHooks.stepTiming(60);
      m_driverController.setXButton(false);
      DriverStationSim.notifyNewData();
      SimHooks.stepTiming(60);
      // Intake should be running
      assertTrue(m_intakeSim.isDeployed());
      assertEquals(IntakeConstants.kIntakeDutyCycle, m_intakeSim.getMotor(), 1e-6);

      // Storage is full
      m_storageSim.setIsFull(true);

      SimHooks.stepTiming(60);
      // Intake and storage should stop
      assertEquals(0.0, m_storageSim.getMotor(), 1e-6);
      assertFalse(m_intakeSim.isDeployed());
      assertEquals(0.0, m_intakeSim.getMotor(), 1e-6);
    }
  }
}
