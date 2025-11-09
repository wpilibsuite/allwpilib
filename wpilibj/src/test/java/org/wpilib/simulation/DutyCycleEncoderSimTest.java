// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.rotation.DutyCycleEncoder;

class DutyCycleEncoderSimTest {
  @Test
  void setTest() {
    try (DutyCycleEncoder encoder = new DutyCycleEncoder(0, 5.67, 0)) {
      DutyCycleEncoderSim sim = new DutyCycleEncoderSim(encoder);

      sim.set(5.67);
      assertEquals(5.67, encoder.get());
    }
  }

  @Test
  void setIsConnectedTest() {
    HAL.initialize(500, 0);

    try (DutyCycleEncoder encoder = new DutyCycleEncoder(0)) {
      DutyCycleEncoderSim sim = new DutyCycleEncoderSim(encoder);

      sim.setConnected(true);
      assertTrue(encoder.isConnected());
      sim.setConnected(false);
      assertFalse(encoder.isConnected());
    }
  }
}
