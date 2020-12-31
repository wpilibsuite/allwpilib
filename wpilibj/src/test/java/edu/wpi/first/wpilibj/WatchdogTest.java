// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class WatchdogTest {
  @BeforeEach
  void setup() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  @Test
  @ResourceLock("timing")
  void enableDisableTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    try (Watchdog watchdog =
        new Watchdog(
            0.4,
            () -> {
              watchdogCounter.addAndGet(1);
            })) {
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

    try (Watchdog watchdog =
        new Watchdog(
            0.4,
            () -> {
              watchdogCounter.addAndGet(1);
            })) {
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

    try (Watchdog watchdog =
        new Watchdog(
            1.0,
            () -> {
              watchdogCounter.addAndGet(1);
            })) {
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

    try (Watchdog watchdog =
        new Watchdog(
            0.4,
            () -> {
              watchdogCounter.addAndGet(1);
            })) {
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

    try (Watchdog watchdog1 =
            new Watchdog(
                0.2,
                () -> {
                  watchdogCounter1.addAndGet(1);
                });
        Watchdog watchdog2 =
            new Watchdog(
                0.6,
                () -> {
                  watchdogCounter2.addAndGet(1);
                })) {
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
