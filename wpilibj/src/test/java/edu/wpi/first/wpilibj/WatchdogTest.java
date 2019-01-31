/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.atomic.AtomicInteger;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class WatchdogTest {
  @Test
  void enableDisableTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    final Watchdog watchdog = new Watchdog(0.4, () -> {
      watchdogCounter.addAndGet(1);
    });

    System.out.println("Run 1");
    watchdog.enable();
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.disable();

    assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");

    System.out.println("Run 2");
    watchdogCounter.set(0);
    watchdog.enable();
    try {
      Thread.sleep(600);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.disable();

    assertEquals(1, watchdogCounter.get(),
        "Watchdog either didn't trigger or triggered more than once");

    // Run 3
    watchdogCounter.set(0);
    watchdog.enable();
    try {
      Thread.sleep(1000);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.disable();

    assertEquals(1, watchdogCounter.get(),
        "Watchdog either didn't trigger or triggered more than once");
  }

  @Test
  void resetTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    final Watchdog watchdog = new Watchdog(0.4, () -> {
      watchdogCounter.addAndGet(1);
    });

    watchdog.enable();
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.reset();
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.disable();

    assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");
  }

  @Test
  void setTimeoutTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    final Watchdog watchdog = new Watchdog(1.0, () -> {
      watchdogCounter.addAndGet(1);
    });

    watchdog.enable();
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.setTimeout(0.2);

    assertEquals(0.2, watchdog.getTimeout());
    assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");

    try {
      Thread.sleep(300);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.disable();

    assertEquals(1, watchdogCounter.get(),
        "Watchdog either didn't trigger or triggered more than once");
  }

  @Test
  void isExpiredTest() {
    final Watchdog watchdog = new Watchdog(0.2, () -> {
    });
    assertFalse(watchdog.isExpired());
    watchdog.enable();

    assertFalse(watchdog.isExpired());
    try {
      Thread.sleep(300);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    assertTrue(watchdog.isExpired());

    watchdog.disable();
    assertTrue(watchdog.isExpired());

    watchdog.reset();
    assertFalse(watchdog.isExpired());
  }

  @Test
  void epochsTest() {
    final AtomicInteger watchdogCounter = new AtomicInteger(0);

    final Watchdog watchdog = new Watchdog(0.4, () -> {
      watchdogCounter.addAndGet(1);
    });

    System.out.println("Run 1");
    watchdog.enable();
    watchdog.addEpoch("Epoch 1");
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.addEpoch("Epoch 2");
    try {
      Thread.sleep(100);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.addEpoch("Epoch 3");
    watchdog.disable();

    assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");

    System.out.println("Run 2");
    watchdog.enable();
    watchdog.addEpoch("Epoch 1");
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.reset();
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog.addEpoch("Epoch 2");
    watchdog.disable();

    assertEquals(0, watchdogCounter.get(), "Watchdog triggered early");
  }

  @Test
  void multiWatchdogTest() {
    final AtomicInteger watchdogCounter1 = new AtomicInteger(0);
    final AtomicInteger watchdogCounter2 = new AtomicInteger(0);

    final Watchdog watchdog1 = new Watchdog(0.2, () -> {
      watchdogCounter1.addAndGet(1);
    });
    final Watchdog watchdog2 = new Watchdog(0.6, () -> {
      watchdogCounter2.addAndGet(1);
    });

    watchdog2.enable();
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    assertEquals(0, watchdogCounter1.get(), "Watchdog triggered early");
    assertEquals(0, watchdogCounter2.get(), "Watchdog triggered early");

    // Sleep enough such that only the watchdog enabled later times out first
    watchdog1.enable();
    try {
      Thread.sleep(300);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    watchdog1.disable();
    watchdog2.disable();

    assertEquals(1, watchdogCounter1.get(),
        "Watchdog either didn't trigger or triggered more than once");
    assertEquals(0, watchdogCounter2.get(), "Watchdog triggered early");
  }
}
