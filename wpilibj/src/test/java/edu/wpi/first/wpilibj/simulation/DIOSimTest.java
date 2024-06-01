// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DigitalInput;
import edu.wpi.first.wpilibj.DigitalOutput;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import org.junit.jupiter.api.Test;

class DIOSimTest {
  @Test
  void testInitialization() {
    DIOSim sim = new DIOSim(2);
    assertFalse(sim.getInitialized());

    BooleanCallback initializedCallback = new BooleanCallback();
    BooleanCallback isInputCallback = new BooleanCallback();

    try (CallbackStore initializeCb = sim.registerInitializedCallback(initializedCallback, false);
        CallbackStore inputCb = sim.registerIsInputCallback(isInputCallback, false);
        DigitalOutput output = new DigitalOutput(2)) {
      assertTrue(sim.getInitialized());
      assertTrue(initializedCallback.wasTriggered());
      assertTrue(initializedCallback.getSetValue());

      assertFalse(sim.getIsInput());
      assertTrue(isInputCallback.wasTriggered());
      assertFalse(isInputCallback.getSetValue());

      initializedCallback.reset();
      sim.setInitialized(false);
      assertTrue(initializedCallback.wasTriggered());
      assertFalse(initializedCallback.getSetValue());
    }
  }

  @Test
  void testInput() {
    HAL.initialize(500, 0);

    try (DigitalInput input = new DigitalInput(0)) {
      DIOSim sim = new DIOSim(input);
      assertTrue(sim.getIsInput());

      BooleanCallback valueCallback = new BooleanCallback();

      try (CallbackStore cb = sim.registerValueCallback(valueCallback, false)) {
        assertTrue(input.get());
        assertTrue(sim.getValue());

        assertFalse(valueCallback.wasTriggered());
        sim.setValue(false);
        assertTrue(valueCallback.wasTriggered());
        assertFalse(valueCallback.getSetValue());
      }
    }
  }

  @Test
  void testOutput() {
    HAL.initialize(500, 0);
    try (DigitalOutput output = new DigitalOutput(0)) {
      DIOSim sim = new DIOSim(output);
      assertFalse(sim.getIsInput());

      BooleanCallback valueCallback = new BooleanCallback();

      try (CallbackStore cb = sim.registerValueCallback(valueCallback, false)) {
        assertTrue(output.get());
        assertTrue(sim.getValue());

        assertFalse(valueCallback.wasTriggered());
        output.set(false);
        assertTrue(valueCallback.wasTriggered());
        assertFalse(valueCallback.getSetValue());
      }
    }
  }
}
