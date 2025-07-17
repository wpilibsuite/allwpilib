// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DutyCycle;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import org.junit.jupiter.api.Test;

class DutyCycleSimTest {
  @Test
  void testInitialization() {
    DutyCycleSim sim = DutyCycleSim.createForChannel(2);
    assertFalse(sim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(callback, false);
        DutyCycle dc = new DutyCycle(2)) {
      assertTrue(sim.getInitialized());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void setFrequencyTest() {
    HAL.initialize(500, 0);

    try (DutyCycle dc = new DutyCycle(2)) {
      DoubleCallback callback = new DoubleCallback();
      DutyCycleSim sim = new DutyCycleSim(dc);
      try (CallbackStore cb = sim.registerFrequencyCallback(callback, false)) {
        sim.setFrequency(191);
        assertEquals(191, sim.getFrequency());
        assertEquals(191, dc.getFrequency());
        assertTrue(callback.wasTriggered());
        assertEquals(191, callback.getSetValue());
      }
    }
  }

  @Test
  void setOutputTest() {
    HAL.initialize(500, 0);

    try (DutyCycle dc = new DutyCycle(2)) {
      DoubleCallback callback = new DoubleCallback();
      DutyCycleSim sim = new DutyCycleSim(dc);
      try (CallbackStore cb = sim.registerOutputCallback(callback, false)) {
        sim.setOutput(229.174);
        assertEquals(229.174, sim.getOutput());
        assertEquals(229.174, dc.getOutput());
        assertTrue(callback.wasTriggered());
        assertEquals(229.174, callback.getSetValue());
      }
    }
  }
}
