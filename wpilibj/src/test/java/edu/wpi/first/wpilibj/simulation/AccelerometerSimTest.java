// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

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
    BuiltInAccelerometerSim sim = new BuiltInAccelerometerSim();
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
