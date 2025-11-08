// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.discrete;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.DIOSim;
import org.junit.jupiter.api.Test;

class DigitalOutputTest {
  @Test
  void testDefaultFunctions() {
    try (DigitalOutput output = new DigitalOutput(0)) {
      assertFalse(output.isPulsing());
    }
  }

  @Test
  void testPwmFunctionsWithoutInitialization() {
    HAL.initialize(500, 0);

    try (DigitalOutput output = new DigitalOutput(0)) {
      assertDoesNotThrow(() -> output.updateDutyCycle(0.6));
      assertDoesNotThrow(output::disablePWM);
    }
  }

  @Test
  void testPwmFunctionsWithInitialization() {
    HAL.initialize(500, 0);

    try (DigitalOutput output = new DigitalOutput(0)) {
      DIOSim sim = new DIOSim(output);
      assertEquals(0, sim.getPulseLength());

      output.enablePWM(0.5);
      output.updateDutyCycle(0.6);
    }
  }
}
