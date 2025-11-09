// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.discrete.PWM;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.testutils.BooleanCallback;
import org.wpilib.simulation.testutils.IntCallback;

class PWMSimTest {
  @Test
  void testInitialize() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(callback, false);
        PWM pwm = new PWM(0)) {
      assertTrue(sim.getInitialized());
    }
  }

  @Test
  void testSetPulseTime() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    IntCallback callback = new IntCallback();

    try (CallbackStore cb = sim.registerPulseMicrosecondCallback(callback, false);
        PWM pwm = new PWM(0)) {
      sim.setPulseMicrosecond(2290);
      assertEquals(2290, sim.getPulseMicrosecond());
      assertEquals(2290, pwm.getPulseTimeMicroseconds());
      assertTrue(callback.wasTriggered());
      assertEquals(2290, callback.getSetValue());
    }
  }

  @Test
  void testSetOutputPeriod() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    IntCallback callback = new IntCallback();

    try (CallbackStore cb = sim.registerOutputPeriodCallback(callback, false);
        PWM pwm = new PWM(0)) {
      sim.setOutputPeriod(3504);
      assertEquals(3504, sim.getOutputPeriod());
      assertTrue(callback.wasTriggered());
      assertEquals(3504, callback.getSetValue());
    }
  }
}
