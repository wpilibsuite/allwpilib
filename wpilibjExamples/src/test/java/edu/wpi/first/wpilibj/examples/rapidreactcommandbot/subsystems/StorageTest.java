// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.rapidreactcommandbot.subsystems;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.Constants.StorageConstants;
import edu.wpi.first.wpilibj.examples.rapidreactcommandbot.sim.StorageSim;
import edu.wpi.first.wpilibj.simulation.DriverStationSim;
import edu.wpi.first.wpilibj2.command.Command;
import edu.wpi.first.wpilibj2.command.CommandScheduler;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class StorageTest {
  @BeforeEach
  void setup() {
    m_storage = new Storage();
    m_sim = new StorageSim();
  }

  @AfterEach
  void tearDown() {
    m_storage.close();
    m_sim.reset();
    CommandScheduler.getInstance().cancelAll();
  }

  Storage m_storage; // real subsystem
  StorageSim m_sim; // simulation controller

  @Test
  void storageCommandTest() {
    Command command = m_storage.runCommand();

    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    command.schedule();

    CommandScheduler.getInstance().run();
    m_sim.simulationPeriodic();
    CommandScheduler.getInstance().run();
    m_sim.simulationPeriodic();

    assertEquals(StorageConstants.kStorageDutyCycle, m_sim.getMotor(), 1e-6);
    assertTrue(command.isScheduled());
  }

  @Test
  void idleTest() {
    DriverStationSim.setEnabled(true);
    DriverStationSim.notifyNewData();

    for (int i = 0; i < 150; i++) {
      CommandScheduler.getInstance().run();
      m_sim.simulationPeriodic();
    }

    assertEquals(0.0, m_sim.getMotor(), 1e-6);

    Command idleCommand = m_storage.getCurrentCommand();
    assertEquals("Idle", idleCommand.getName());
  }
}
