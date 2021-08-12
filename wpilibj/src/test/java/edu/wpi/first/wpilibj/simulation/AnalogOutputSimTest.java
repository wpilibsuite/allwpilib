// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.AnalogOutput;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import org.junit.jupiter.api.Test;

class AnalogOutputSimTest {
  @Test
  void testInitialization() {
    HAL.initialize(500, 0);

    AnalogOutputSim outputSim = new AnalogOutputSim(0);
    assertFalse(outputSim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = outputSim.registerInitializedCallback(callback, false);
        AnalogOutput output = new AnalogOutput(0)) {
      assertTrue(outputSim.getInitialized());
    }
  }

  @Test
  void setCallbackTest() {
    HAL.initialize(500, 0);

    try (AnalogOutput output = new AnalogOutput(0)) {
      output.setVoltage(0.5);

      AnalogOutputSim outputSim = new AnalogOutputSim(output);

      DoubleCallback voltageCallback = new DoubleCallback();

      try (CallbackStore cb = outputSim.registerVoltageCallback(voltageCallback, false)) {
        assertFalse(voltageCallback.wasTriggered());

        for (double i = 0.1; i < 5.0; i += 0.1) {
          voltageCallback.reset();

          output.setVoltage(0);

          assertEquals(0, output.getVoltage());
          assertEquals(0, outputSim.getVoltage());
          assertTrue(voltageCallback.wasTriggered());
          assertEquals(voltageCallback.getSetValue(), 0, 0.001);

          voltageCallback.reset();

          output.setVoltage(i);

          assertEquals(i, output.getVoltage());
          assertEquals(i, outputSim.getVoltage());
          assertTrue(voltageCallback.wasTriggered());
          assertEquals(voltageCallback.getSetValue(), i, 0.001);
        }
      }
    }
  }

  @Test
  void testReset() {
    HAL.initialize(500, 0);

    AnalogOutputSim outputSim = new AnalogOutputSim(0);

    try (AnalogOutput output = new AnalogOutput(0)) {
      output.setVoltage(1.2);

      outputSim.resetData();
      assertEquals(0, output.getVoltage());
      assertEquals(0, outputSim.getVoltage());
    }
  }
}
