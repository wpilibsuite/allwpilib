/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.AccelerometerJNI;
import edu.wpi.first.hal.HAL;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class AccelerometerSimTest {
  static class TriggeredStore {
    public boolean m_wasTriggered;
    public boolean m_setValue = true;
  }

  @Test
  void testCallbacks() {
    HAL.initialize(500, 0);
    AccelerometerSim sim = new AccelerometerSim();
    sim.resetData();

    TriggeredStore store = new TriggeredStore();

    try (CallbackStore cb = sim.registerActiveCallback((s, v) -> {
      store.m_wasTriggered = true;
      store.m_setValue = v.getBoolean();
    }, false)) {
      assertFalse(store.m_wasTriggered);
      AccelerometerJNI.setAccelerometerActive(true);
      assertTrue(store.m_wasTriggered);
      assertTrue(store.m_setValue);
    }
  }
}
