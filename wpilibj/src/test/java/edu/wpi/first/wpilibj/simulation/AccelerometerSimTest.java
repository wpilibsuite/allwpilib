// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.BuiltInAccelerometer;
import edu.wpi.first.wpilibj.interfaces.Accelerometer;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.EnumCallback;
import org.junit.jupiter.api.Test;

class AccelerometerSimTest {
  @Test
  void testCallbacks() {
    HAL.initialize(500, 0);
    BuiltInAccelerometerSim sim = new BuiltInAccelerometerSim();
    sim.resetData();

    BooleanCallback store = new BooleanCallback();

    try (CallbackStore cb = sim.registerActiveCallback(store, false)) {
      assertFalse(store.wasTriggered());
      sim.setActive(true);
      assertTrue(sim.getActive());
      assertTrue(store.wasTriggered());
      assertTrue(store.getSetValue());
    }
  }

  @Test
  void testX() {
    HAL.initialize(500, 0);
    BuiltInAccelerometerSim sim = new BuiltInAccelerometerSim();
    sim.resetData();

    DoubleCallback callback = new DoubleCallback();
    final double kTestValue = 1.91;

    try (BuiltInAccelerometer accel = new BuiltInAccelerometer();
        CallbackStore cb = sim.registerXCallback(callback, false)) {
      sim.setX(kTestValue);
      assertEquals(kTestValue, accel.getX());
      assertEquals(kTestValue, sim.getX());
      assertTrue(callback.wasTriggered());
      assertEquals(kTestValue, callback.getSetValue());
    }
  }

  @Test
  void testY() {
    HAL.initialize(500, 0);
    BuiltInAccelerometerSim sim = new BuiltInAccelerometerSim();
    sim.resetData();

    DoubleCallback callback = new DoubleCallback();
    final double kTestValue = 2.29;

    try (BuiltInAccelerometer accel = new BuiltInAccelerometer();
        CallbackStore cb = sim.registerYCallback(callback, false)) {
      sim.setY(kTestValue);
      assertEquals(kTestValue, accel.getY());
      assertEquals(kTestValue, sim.getY());
      assertTrue(callback.wasTriggered());
      assertEquals(kTestValue, callback.getSetValue());
    }
  }

  @Test
  void testZ() {
    HAL.initialize(500, 0);

    BuiltInAccelerometerSim sim = new BuiltInAccelerometerSim();
    sim.resetData();

    DoubleCallback callback = new DoubleCallback();
    final double kTestValue = 3.405;

    try (BuiltInAccelerometer accel = new BuiltInAccelerometer();
        CallbackStore cb = sim.registerZCallback(callback, false)) {
      sim.setZ(kTestValue);
      assertEquals(kTestValue, accel.getZ());
      assertEquals(kTestValue, sim.getZ());
      assertTrue(callback.wasTriggered());
      assertEquals(kTestValue, callback.getSetValue());
    }
  }

  @Test
  void testRange() {
    HAL.initialize(500, 0);

    BuiltInAccelerometerSim sim = new BuiltInAccelerometerSim();
    sim.resetData();

    EnumCallback callback = new EnumCallback();

    Accelerometer.Range range = Accelerometer.Range.k4G;
    try (CallbackStore cb = sim.registerRangeCallback(callback, false);
        BuiltInAccelerometer accel = new BuiltInAccelerometer(range)) {
      assertTrue(callback.wasTriggered());
      assertEquals(range.ordinal(), sim.getRange());
      assertEquals(range.ordinal(), callback.getSetValue());

      // 2G
      callback.reset();
      range = Accelerometer.Range.k2G;
      accel.setRange(range);
      assertTrue(callback.wasTriggered());
      assertEquals(range.ordinal(), sim.getRange());
      assertEquals(range.ordinal(), callback.getSetValue());

      // 4G
      callback.reset();
      range = Accelerometer.Range.k4G;
      accel.setRange(range);
      assertTrue(callback.wasTriggered());
      assertEquals(range.ordinal(), sim.getRange());
      assertEquals(range.ordinal(), callback.getSetValue());

      // 8G
      callback.reset();
      range = Accelerometer.Range.k8G;
      accel.setRange(range);
      assertTrue(callback.wasTriggered());
      assertEquals(range.ordinal(), sim.getRange());
      assertEquals(range.ordinal(), callback.getSetValue());

      // 16G - Not supported
      callback.reset();
      assertThrows(IllegalArgumentException.class, () -> accel.setRange(Accelerometer.Range.k16G));
      assertFalse(callback.wasTriggered());
    }
  }
}
