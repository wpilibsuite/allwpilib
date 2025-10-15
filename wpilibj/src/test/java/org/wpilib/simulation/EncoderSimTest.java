// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.rotation.Encoder;
import org.wpilib.simulation.testutils.DoubleCallback;
import org.wpilib.simulation.testutils.IntCallback;
import org.junit.jupiter.api.Test;

class EncoderSimTest {
  private static final double DEFAULT_DISTANCE_PER_PULSE = 0.0005;

  @Test
  void testRate() {
    HAL.initialize(500, 0);

    try (Encoder encoder = new Encoder(0, 1)) {
      EncoderSim sim = new EncoderSim(encoder);
      sim.resetData();

      encoder.setDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

      sim.setRate(1.91);
      assertEquals(1.91, sim.getRate());
    }
  }

  @Test
  void testCount() {
    HAL.initialize(500, 0);

    try (Encoder encoder = new Encoder(0, 1)) {
      EncoderSim sim = new EncoderSim(encoder);
      sim.resetData();

      encoder.setDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

      IntCallback callback = new IntCallback();
      try (CallbackStore cb = sim.registerCountCallback(callback, false)) {
        sim.setCount(3504);
        assertEquals(3504, sim.getCount());

        assertTrue(callback.wasTriggered());
        assertEquals(3504, encoder.get());
        assertEquals(3504, callback.getSetValue());
      }
    }
  }

  @Test
  void testDistance() {
    HAL.initialize(500, 0);

    try (Encoder encoder = new Encoder(0, 1)) {
      EncoderSim sim = new EncoderSim(encoder);
      sim.resetData();

      encoder.setDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

      sim.setDistance(229.174);
      assertEquals(229.174, sim.getDistance());
      assertEquals(229.174, encoder.getDistance());
    }
  }

  @SuppressWarnings("deprecation") // Encoder.getPeriod()
  @Test
  void testPeriod() {
    HAL.initialize(500, 0);

    try (Encoder encoder = new Encoder(0, 1)) {
      EncoderSim sim = new EncoderSim(encoder);
      sim.resetData();

      encoder.setDistancePerPulse(DEFAULT_DISTANCE_PER_PULSE);

      DoubleCallback callback = new DoubleCallback();
      try (CallbackStore cb = sim.registerPeriodCallback(callback, false)) {
        sim.setPeriod(123.456);
        assertEquals(123.456, sim.getPeriod());
        assertEquals(123.456, encoder.getPeriod());
        assertEquals(DEFAULT_DISTANCE_PER_PULSE / 123.456, encoder.getRate());
      }
    }
  }

  @Test
  void testDistancePerPulse() {
    HAL.initialize(500, 0);

    try (Encoder encoder = new Encoder(0, 1)) {
      EncoderSim sim = new EncoderSim(encoder);
      sim.resetData();

      DoubleCallback callback = new DoubleCallback();
      try (CallbackStore cb = sim.registerDistancePerPulseCallback(callback, false)) {
        sim.setDistancePerPulse(0.03405);
        assertEquals(0.03405, sim.getDistancePerPulse());
        assertEquals(0.03405, encoder.getDistancePerPulse());
        assertTrue(callback.wasTriggered());
        assertEquals(0.03405, callback.getSetValue());
      }
    }
  }
}
