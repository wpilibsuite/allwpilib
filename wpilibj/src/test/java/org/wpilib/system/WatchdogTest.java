// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.hardware.hal.HALUtil;
import org.wpilib.hardware.hal.simulation.SimulatorJNI;
import org.wpilib.simulation.SimHooks;

class WatchdogTest {
  @BeforeEach
  void setup() {
    HAL.initialize();
    SimHooks.pauseTiming();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  /**
   * Steps the paused clock to an odd nanosecond count of at least 2^53. Seconds in a double can't
   * represent it, so it catches rounding from converting timestamps through seconds.
   */
  private static void stepToLargeClock() {
    long now = HALUtil.getMonotonicTime();
    long target = Math.max(now, 1L << 53) | 1;
    SimulatorJNI.stepTiming(target - now);
  }

  @Test
  @ResourceLock("timing")
  void largeClockTest() {
    stepToLargeClock();

    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    try (Watchdog watchdog = new Watchdog(0.4, () -> watchdogCounter.addAndGet(1))) {
      watchdog.enable();
      SimulatorJNI.stepTiming(399_999_999L);
      assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");
      SimulatorJNI.stepTiming(1L);
      assertEquals(1, watchdogCounter.get(), "Watchdog didn't trigger at the timeout");

      watchdogCounter.set(0);
      watchdog.setTimeout(0.4);
      SimulatorJNI.stepTiming(200_000_000L);
      assertEquals(0.2, watchdog.getTime(), 1e-10);
      SimulatorJNI.stepTiming(199_999_999L);
      assertEquals(0, watchdogCounter.get(), "Watchdog triggered early after setTimeout");
      SimulatorJNI.stepTiming(1L);
      assertEquals(
          1, watchdogCounter.get(), "Watchdog didn't trigger at the timeout after setTimeout");

      watchdog.disable();
    }
  }

  @Test
  @ResourceLock("timing")
  void enableDisableTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    try (Watchdog watchdog = new Watchdog(0.4, () -> watchdogCounter.addAndGet(1))) {
      // Run 1
      watchdog.enable();
      SimHooks.stepTiming(0.2);
      watchdog.disable();

      assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");

      // Run 2
      watchdogCounter.set(0);
      watchdog.enable();
      SimHooks.stepTiming(0.4);
      watchdog.disable();

      assertEquals(
          1, watchdogCounter.get(), "Watchdog either didn't trigger or triggered more than once");

      // Run 3
      watchdogCounter.set(0);
      watchdog.enable();
      SimHooks.stepTiming(1.0);
      watchdog.disable();

      assertEquals(
          1, watchdogCounter.get(), "Watchdog either didn't trigger or triggered more than once");
    }
  }

  @Test
  @ResourceLock("timing")
  void resetTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    try (Watchdog watchdog = new Watchdog(0.4, () -> watchdogCounter.addAndGet(1))) {
      watchdog.enable();
      SimHooks.stepTiming(0.2);
      watchdog.reset();
      SimHooks.stepTiming(0.2);
      watchdog.disable();

      assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");
    }
  }

  @Test
  @ResourceLock("timing")
  void setTimeoutTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    try (Watchdog watchdog = new Watchdog(1.0, () -> watchdogCounter.addAndGet(1))) {
      watchdog.enable();
      SimHooks.stepTiming(0.2);
      watchdog.setTimeout(0.2);

      assertEquals(0.2, watchdog.getTimeout());
      assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");

      SimHooks.stepTiming(0.3);
      watchdog.disable();

      assertEquals(
          1, watchdogCounter.get(), "Watchdog either didn't trigger or triggered more than once");
    }
  }

  @Test
  @ResourceLock("timing")
  void isExpiredTest() {
    try (Watchdog watchdog = new Watchdog(0.2, () -> {})) {
      assertFalse(watchdog.isExpired());
      watchdog.enable();

      assertFalse(watchdog.isExpired());
      SimHooks.stepTiming(0.3);
      assertTrue(watchdog.isExpired());

      watchdog.disable();
      assertTrue(watchdog.isExpired());

      watchdog.reset();
      assertFalse(watchdog.isExpired());
    }
  }

  @Test
  @ResourceLock("timing")
  void epochsTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    try (Watchdog watchdog = new Watchdog(0.4, () -> watchdogCounter.addAndGet(1))) {
      // Run 1
      watchdog.enable();
      watchdog.addEpoch("Epoch 1");
      SimHooks.stepTiming(0.1);
      watchdog.addEpoch("Epoch 2");
      SimHooks.stepTiming(0.1);
      watchdog.addEpoch("Epoch 3");
      watchdog.disable();

      assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");

      // Run 2
      watchdog.enable();
      watchdog.addEpoch("Epoch 1");
      SimHooks.stepTiming(0.2);
      watchdog.reset();
      SimHooks.stepTiming(0.2);
      watchdog.addEpoch("Epoch 2");
      watchdog.disable();

      assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");
    }
  }

  @Test
  @ResourceLock("timing")
  void multiWatchdogTest() {
    final AtomicInteger watchdogCounter1 = new AtomicInteger(0);
    final AtomicInteger watchdogCounter2 = new AtomicInteger(0);

    try (Watchdog watchdog1 = new Watchdog(0.2, () -> watchdogCounter1.addAndGet(1));
        Watchdog watchdog2 = new Watchdog(0.6, () -> watchdogCounter2.addAndGet(1))) {
      watchdog2.enable();
      SimHooks.stepTiming(0.25);
      assertEquals(0, watchdogCounter1.get(), "Watchdog triggered early");
      assertEquals(0, watchdogCounter2.get(), "Watchdog triggered early");

      // Sleep enough such that only the watchdog enabled later times out first
      watchdog1.enable();
      SimHooks.stepTiming(0.25);
      watchdog1.disable();
      watchdog2.disable();

      assertEquals(
          1, watchdogCounter1.get(), "Watchdog either didn't trigger or triggered more than once");
      assertEquals(0, watchdogCounter2.get(), "Watchdog triggered early");
    }
  }
}
