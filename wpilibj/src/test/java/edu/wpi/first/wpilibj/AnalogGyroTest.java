// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.AnalogGyroSim;
import org.junit.jupiter.api.Test;

class AnalogGyroTest {
  @Test
  void testInitializeWithAnalogInput() {
    HAL.initialize(500, 0);
    AnalogGyroSim sim = new AnalogGyroSim(0);
    assertFalse(sim.getInitialized());

    try (AnalogInput ai = new AnalogInput(0);
        AnalogGyro gyro = new AnalogGyro(ai, 229, 17.4)) {
      assertEquals(ai, gyro.getAnalogInput());
    }
  }

  @Test
  void testInitializeWithChannel() {
    HAL.initialize(500, 0);
    AnalogGyroSim sim = new AnalogGyroSim(0);
    assertFalse(sim.getInitialized());

    try (AnalogGyro gyro = new AnalogGyro(1, 191, 35.04)) {
      assertNotNull(gyro.getAnalogInput());
    }
  }
}
