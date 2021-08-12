// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.IntCallback;
import org.junit.jupiter.api.Test;

class DigitalPWMSimTest {
  @Test
  void testInitialization() {
    try (DigitalOutput output = new DigitalOutput(0)) {
      DigitalPWMSim sim = new DigitalPWMSim(output);
      assertFalse(sim.getInitialized());

      BooleanCallback initializeCallback = new BooleanCallback();
      DoubleCallback dutyCycleCallback = new DoubleCallback();
      try (CallbackStore initCb = sim.registerInitializedCallback(initializeCallback, false);
          CallbackStore dutyCycleCb = sim.registerDutyCycleCallback(dutyCycleCallback, false); ) {
        final double kTestDutyCycle = 0.191;
        output.enablePWM(kTestDutyCycle);

        assertTrue(sim.getInitialized());
        assertTrue(initializeCallback.wasTriggered());
        assertTrue(initializeCallback.getSetValue());

        assertEquals(kTestDutyCycle, sim.getDutyCycle());
        assertTrue(dutyCycleCallback.wasTriggered());
        assertEquals(kTestDutyCycle, dutyCycleCallback.getSetValue());
      }
    }
  }

  @Test
  void setPinTest() {
    HAL.initialize(500, 0);

    try (DigitalOutput output = new DigitalOutput(0)) {
      DigitalPWMSim sim = new DigitalPWMSim(output);
      IntCallback callback = new IntCallback();
      try (CallbackStore cb = sim.registerPinCallback(callback, false)) {
        sim.setPin(191);
        assertEquals(191, sim.getPin());
        assertTrue(callback.wasTriggered());
        assertEquals(191, callback.getSetValue());
      }
    }
  }
}
