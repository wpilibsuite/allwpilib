// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.PWM;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.IntCallback;
import org.junit.jupiter.api.Test;

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
  void testSetRawValue() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    IntCallback callback = new IntCallback();

    try (CallbackStore cb = sim.registerRawValueCallback(callback, false);
        PWM pwm = new PWM(0)) {
      sim.setRawValue(229);
      assertEquals(229, sim.getRawValue());
      assertEquals(229, pwm.getRaw());
      assertTrue(callback.wasTriggered());
      assertEquals(229, callback.getSetValue());
    }
  }

  @Test
  void testSetSpeed() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    DoubleCallback callback = new DoubleCallback();

    try (CallbackStore cb = sim.registerSpeedCallback(callback, false);
        PWM pwm = new PWM(0)) {
      final double kTestValue = 0.3504;
      pwm.setSpeed(kTestValue);

      assertEquals(kTestValue, sim.getSpeed());
      assertEquals(kTestValue, pwm.getSpeed());
      assertTrue(callback.wasTriggered());
      assertEquals(kTestValue, callback.getSetValue());
    }
  }

  @Test
  void testSetPosition() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    DoubleCallback callback = new DoubleCallback();

    try (CallbackStore cb = sim.registerPositionCallback(callback, false);
        PWM pwm = new PWM(0)) {
      final double kTestValue = 0.3504;
      pwm.setPosition(kTestValue);

      assertEquals(kTestValue, sim.getPosition());
      assertEquals(kTestValue, pwm.getPosition());
      assertTrue(callback.wasTriggered());
      assertEquals(kTestValue, callback.getSetValue());
    }
  }

  @Test
  void testSetPeriodScale() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    IntCallback callback = new IntCallback();

    try (CallbackStore cb = sim.registerPeriodScaleCallback(callback, false);
        PWM pwm = new PWM(0)) {
      sim.setPeriodScale(3504);
      assertEquals(3504, sim.getPeriodScale());
      assertTrue(callback.wasTriggered());
      assertEquals(3504, callback.getSetValue());
    }
  }

  @Test
  void testSetZeroLatch() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerZeroLatchCallback(callback, false);
        PWM pwm = new PWM(0)) {
      pwm.setZeroLatch();

      assertTrue(callback.wasTriggered());
    }
  }
}
