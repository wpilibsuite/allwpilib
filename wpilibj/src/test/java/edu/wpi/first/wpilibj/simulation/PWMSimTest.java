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
  private static final double PWM_STEP_SIZE = 1.0 / 2000.0;

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
  void testSetSpeed() {
    HAL.initialize(500, 0);

    PWMSim sim = new PWMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    DoubleCallback callback = new DoubleCallback();

    try (CallbackStore cb = sim.registerSpeedCallback(callback, false);
        PWM pwm = new PWM(0)) {
      double kTestValue = 0.3504;
      pwm.setSpeed(kTestValue);

      assertEquals(kTestValue, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getSpeed(), PWM_STEP_SIZE);
      assertTrue(callback.wasTriggered());
      assertEquals(kTestValue, callback.getSetValue(), PWM_STEP_SIZE);
      assertEquals(kTestValue / 2 + 0.5, sim.getPosition(), PWM_STEP_SIZE * 2);
      assertEquals(kTestValue / 2 + 0.5, pwm.getPosition(), PWM_STEP_SIZE * 2);

      kTestValue = -1.0;
      pwm.setSpeed(kTestValue);

      assertEquals(kTestValue, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getSpeed(), PWM_STEP_SIZE);
      assertEquals(0.0, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(0.0, pwm.getPosition(), PWM_STEP_SIZE);

      kTestValue = 0.0;
      pwm.setSpeed(kTestValue);

      assertEquals(kTestValue, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getSpeed(), PWM_STEP_SIZE);
      assertEquals(0.5, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(0.5, pwm.getPosition(), PWM_STEP_SIZE);

      kTestValue = 1.0;
      pwm.setSpeed(kTestValue);

      assertEquals(kTestValue, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getSpeed(), PWM_STEP_SIZE);
      assertEquals(kTestValue, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getPosition(), PWM_STEP_SIZE);

      kTestValue = 1.1;
      pwm.setSpeed(kTestValue);

      assertEquals(1.0, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(1.0, pwm.getSpeed(), PWM_STEP_SIZE);
      assertEquals(1.0, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(1.0, pwm.getPosition(), PWM_STEP_SIZE);
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
      double kTestValue = 0.3504;
      pwm.setPosition(kTestValue);

      assertEquals(kTestValue, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getPosition(), PWM_STEP_SIZE);
      assertTrue(callback.wasTriggered());
      assertEquals(kTestValue, callback.getSetValue(), PWM_STEP_SIZE);
      assertEquals(kTestValue * 2 - 1.0, sim.getSpeed(), PWM_STEP_SIZE * 2);
      assertEquals(kTestValue * 2 - 1.0, pwm.getSpeed(), PWM_STEP_SIZE * 2);

      kTestValue = -1.0;
      pwm.setPosition(kTestValue);

      assertEquals(0.0, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(0.0, pwm.getPosition(), PWM_STEP_SIZE);
      assertEquals(kTestValue, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getSpeed(), PWM_STEP_SIZE);

      kTestValue = 0.0;
      pwm.setPosition(kTestValue);

      assertEquals(kTestValue, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getPosition(), PWM_STEP_SIZE);
      assertEquals(-1.0, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(-1.0, pwm.getSpeed(), PWM_STEP_SIZE);

      kTestValue = 1.0;
      pwm.setPosition(kTestValue);

      assertEquals(kTestValue, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getPosition(), PWM_STEP_SIZE);
      assertEquals(kTestValue, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(kTestValue, pwm.getSpeed(), PWM_STEP_SIZE);

      kTestValue = 1.1;
      pwm.setPosition(kTestValue);

      assertEquals(1.0, sim.getPosition(), PWM_STEP_SIZE);
      assertEquals(1.0, pwm.getPosition(), PWM_STEP_SIZE);
      assertEquals(1.0, sim.getSpeed(), PWM_STEP_SIZE);
      assertEquals(1.0, pwm.getSpeed(), PWM_STEP_SIZE);
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
