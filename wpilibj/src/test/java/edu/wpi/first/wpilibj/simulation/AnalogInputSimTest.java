// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.util.AllocationException;
import edu.wpi.first.wpilibj.AnalogInput;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import org.junit.jupiter.api.Test;

class AnalogInputSimTest {
  @Test
  void setInitializeTest() {
    HAL.initialize(500, 0);

    AnalogInputSim sim = new AnalogInputSim(5);
    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(callback, false);
        AnalogInput input = new AnalogInput(5)) {
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void testSetVoltage() {
    HAL.initialize(500, 0);

    AnalogInputSim sim = new AnalogInputSim(5);
    DoubleCallback callback = new DoubleCallback();

    try (CallbackStore cb = sim.registerVoltageCallback(callback, false);
        AnalogInput input = new AnalogInput(5)) {
      // Bootstrap the voltage to be non-zero
      sim.setVoltage(1.0);

      for (double i = 0; i < 5.0; i += 0.1) {
        callback.reset();

        sim.setVoltage(0);
        assertEquals(input.getVoltage(), 0, 0.001);

        callback.reset();
        sim.setVoltage(i);
        assertEquals(input.getVoltage(), i, 0.001);
      }
    }
  }

  @Test
  void testSetOverSampleBits() {
    HAL.initialize(500, 0);
    try (AnalogInput input = new AnalogInput(5)) {
      input.setOversampleBits(3504);
      assertEquals(3504, input.getOversampleBits());
    }
  }

  @Test
  void tesInitAccumulator() {
    HAL.initialize(500, 0);
    try (AnalogInput input = new AnalogInput(0)) {
      // First initialization works fine
      assertDoesNotThrow(input::initAccumulator);

      input.resetAccumulator();
    }
  }

  @Test
  void tesInitAccumulatorOnInvalidPort() {
    HAL.initialize(500, 0);
    try (AnalogInput input = new AnalogInput(5)) {
      assertThrows(AllocationException.class, input::initAccumulator);
    }
  }
}
