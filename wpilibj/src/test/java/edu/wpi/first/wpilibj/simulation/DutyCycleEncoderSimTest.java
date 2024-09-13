// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DutyCycleEncoder;
import org.junit.jupiter.api.Test;

class DutyCycleEncoderSimTest {
  @Test
  void setTest() {
    try (DutyCycleEncoder encoder = new DutyCycleEncoder(0)) {
      DutyCycleEncoderSim sim = new DutyCycleEncoderSim(encoder);

      sim.set(5.67);
      assertEquals(5.67, encoder.get());
    }
  }

  @Test
  void setDistanceTest() {
    HAL.initialize(500, 0);

    try (DutyCycleEncoder encoder = new DutyCycleEncoder(0)) {
      DutyCycleEncoderSim sim = new DutyCycleEncoderSim(encoder);

      sim.setDistance(19.1);
      assertEquals(19.1, encoder.getDistance());
    }
  }

  @Test
  void setDistancePerRotationTest() {
    HAL.initialize(500, 0);

    try (DutyCycleEncoder encoder = new DutyCycleEncoder(0)) {
      DutyCycleEncoderSim sim = new DutyCycleEncoderSim(encoder);
      sim.set(1.5);
      encoder.setDistancePerRotation(42);
      assertEquals(63.0, encoder.getDistance());
    }
  }

  @Test
  void setAbsolutePositionTest() {
    HAL.initialize(500, 0);

    try (DutyCycleEncoder encoder = new DutyCycleEncoder(0)) {
      DutyCycleEncoderSim sim = new DutyCycleEncoderSim(encoder);

      sim.setAbsolutePosition(0.75);
      assertEquals(0.75, encoder.getAbsolutePosition());
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

  @Test
  void resetTest() {
    HAL.initialize(500, 0);

    try (DutyCycleEncoder encoder = new DutyCycleEncoder(0)) {
      DutyCycleEncoderSim sim = new DutyCycleEncoderSim(encoder);

      sim.setDistance(2.5);
      assertEquals(2.5, encoder.getDistance());
      encoder.reset();
      assertEquals(0.0, encoder.getDistance());
    }
  }
}
