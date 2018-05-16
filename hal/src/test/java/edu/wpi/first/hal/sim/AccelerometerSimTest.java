/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import edu.wpi.first.wpilibj.hal.AccelerometerJNI;
import edu.wpi.first.wpilibj.hal.HAL;
import edu.wpi.first.wpilibj.sim.AccelerometerSim;
import edu.wpi.first.wpilibj.sim.CallbackStore;


public class AccelerometerSimTest {
  static class TriggeredStore {
    public boolean wasTriggered = false;
    public boolean setValue = true;
  }

  @Test
  public void testCallbacks() {
    HAL.initialize(500, 0);
    AccelerometerSim sim = new AccelerometerSim();
    sim.resetData();

    TriggeredStore store = new TriggeredStore();

    try (CallbackStore cb = sim.registerActiveCallback((s, v) -> {
      store.wasTriggered = true;
      store.setValue = v.getBoolean();
    }, false)) {
      assertFalse(store.wasTriggered);
      AccelerometerJNI.setAccelerometerActive(true);
      assertTrue(store.wasTriggered);
      assertTrue(store.setValue);
    }
  }
}
