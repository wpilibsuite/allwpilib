// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.AnalogTrigger;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import org.junit.jupiter.api.Test;

class AnalogTriggerSimTest {
  @Test
  void testInitialization() {
    HAL.initialize(500, 0);

    AnalogTriggerSim sim = AnalogTriggerSim.createForIndex(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(callback, false);
        AnalogTrigger trigger = new AnalogTrigger(0)) {
      assertTrue(sim.getInitialized());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void triggerLowerBoundTest() {
    HAL.initialize(500, 0);

    try (AnalogTrigger trigger = new AnalogTrigger(0)) {
      AnalogTriggerSim sim = new AnalogTriggerSim(trigger);
      DoubleCallback lowerCallback = new DoubleCallback();
      DoubleCallback upperCallback = new DoubleCallback();
      try (CallbackStore lowerCb = sim.registerTriggerLowerBoundCallback(lowerCallback, false);
          CallbackStore upperCb = sim.registerTriggerUpperBoundCallback(upperCallback, false)) {
        trigger.setLimitsVoltage(0.299, 1.91);

        assertEquals(0.299, sim.getTriggerLowerBound());
        assertEquals(1.91, sim.getTriggerUpperBound());

        assertTrue(lowerCallback.wasTriggered());
        assertEquals(0.299, lowerCallback.getSetValue());

        assertTrue(upperCallback.wasTriggered());
        assertEquals(1.91, upperCallback.getSetValue());
      }
    }
  }
}
