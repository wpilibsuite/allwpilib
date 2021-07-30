// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.AnalogGyro;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import org.junit.jupiter.api.Test;

class AnalogGyroSimTest {
  @Test
  void testInitialization() {
    HAL.initialize(500, 0);
    AnalogGyroSim sim = new AnalogGyroSim(0);
    assertFalse(sim.getInitialized());

    BooleanCallback initializedCallback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(initializedCallback, false);
        AnalogInput ai = new AnalogInput(0);
        AnalogGyro gyro = new AnalogGyro(ai)) {
      assertTrue(sim.getInitialized());
      assertTrue(initializedCallback.wasTriggered());
      assertTrue(initializedCallback.getSetValue());
    }
  }

  @Test
  void testSetAngle() {
    HAL.initialize(500, 0);

    AnalogGyroSim sim = new AnalogGyroSim(0);
    DoubleCallback callback = new DoubleCallback();

    try (AnalogInput ai = new AnalogInput(0);
        AnalogGyro gyro = new AnalogGyro(ai);
        CallbackStore cb = sim.registerAngleCallback(callback, false)) {
      assertEquals(0, gyro.getAngle());

      final double TEST_ANGLE = 35.04;
      sim.setAngle(TEST_ANGLE);
      assertEquals(TEST_ANGLE, sim.getAngle());
      assertEquals(TEST_ANGLE, gyro.getAngle());
      assertEquals(-TEST_ANGLE, gyro.getRotation2d().getDegrees());
      assertTrue(callback.wasTriggered());
      assertEquals(TEST_ANGLE, callback.getSetValue());
    }
  }

  @Test
  void testSetRate() {
    HAL.initialize(500, 0);

    AnalogGyroSim sim = new AnalogGyroSim(0);
    DoubleCallback callback = new DoubleCallback();

    try (AnalogInput ai = new AnalogInput(0);
        AnalogGyro gyro = new AnalogGyro(ai);
        CallbackStore cb = sim.registerRateCallback(callback, false)) {
      assertEquals(0, gyro.getRate());

      final double TEST_RATE = -19.1;
      sim.setRate(TEST_RATE);
      assertEquals(TEST_RATE, sim.getRate());
      assertEquals(TEST_RATE, gyro.getRate());

      assertTrue(callback.wasTriggered());
      assertEquals(TEST_RATE, callback.getSetValue());
    }
  }

  @Test
  void testReset() {
    HAL.initialize(500, 0);

    try (AnalogInput ai = new AnalogInput(0);
        AnalogGyro gyro = new AnalogGyro(ai)) {
      AnalogGyroSim sim = new AnalogGyroSim(gyro);
      sim.setAngle(12.34);
      sim.setRate(43.21);

      sim.resetData();
      assertEquals(0, sim.getAngle());
      assertEquals(0, sim.getRate());
      assertEquals(0, gyro.getAngle());
      assertEquals(0, gyro.getRate());
    }
  }
}
