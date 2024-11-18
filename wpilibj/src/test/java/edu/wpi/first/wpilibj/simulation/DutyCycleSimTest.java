// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.wpilib.hal.HAL;
import org.wpilib.wpilibj.DigitalInput;
import org.wpilib.wpilibj.DutyCycle;
import org.wpilib.wpilibj.simulation.testutils.BooleanCallback;
import org.wpilib.wpilibj.simulation.testutils.DoubleCallback;
import org.wpilib.wpilibj.simulation.testutils.IntCallback;
import org.junit.jupiter.api.Test;

class DutyCycleSimTest {
  @Test
  void testInitialization() {
    DutyCycleSim sim = DutyCycleSim.createForIndex(0);
    assertFalse(sim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(callback, false);
        DigitalInput di = new DigitalInput(2);
        DutyCycle dc = new DutyCycle(di)) {
      assertTrue(sim.getInitialized());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void setFrequencyTest() {
    HAL.initialize(500, 0);

    try (DigitalInput di = new DigitalInput(2);
        DutyCycle dc = new DutyCycle(di)) {
      IntCallback callback = new IntCallback();
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

    try (DigitalInput di = new DigitalInput(2);
        DutyCycle dc = new DutyCycle(di)) {
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
