/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import edu.wpi.first.wpilibj.AnalogOutput;
import edu.wpi.first.wpilibj.hal.HAL;

public class AnalogOutputSimTest {
  static class DoubleStore {
    public boolean wasTriggered = false;
    public boolean wasCorrectType = false;
    public double setValue = -1;

    public void reset() {
      wasCorrectType = false;
      wasTriggered = false;
      setValue = -1;
    }
  }

  @Test
  public void testSetCallback() {
    HAL.initialize(500, 0);


    AnalogOutput output = new AnalogOutput(0);
    output.setVoltage(0.5);

    AnalogOutSim outputSim = output.getSimObject();

    DoubleStore store = new DoubleStore();

    try (CallbackStore cb = outputSim.registerVoltageCallback((s, v) -> {
      store.wasTriggered = true;
      store.wasCorrectType = true;
      store.setValue = v.getDouble();
    }, false)) {
      assertFalse(store.wasTriggered);

      for (double i = 0.1; i < 5.0; i+=0.1) {
        store.reset();

        output.setVoltage(0);

        assertTrue(store.wasTriggered);
        assertEquals(store.setValue, 0, 0.001);

        store.reset();

        output.setVoltage(i);

        assertTrue(store.wasTriggered);
        assertEquals(store.setValue, i, 0.001);

      }
    }
  }
}
