/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.concurrent.atomic.AtomicInteger;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class FiberSchedulerTest {
  @Test
  void addRemoveTest() {
    final AtomicInteger fiberCounter = new AtomicInteger(0);

    final FiberScheduler scheduler = new FiberScheduler();
    final Fiber fiber = new Fiber(0.4, () -> {
      fiberCounter.addAndGet(1);
    });

    System.out.println("Run 1");
    scheduler.add(fiber);
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    scheduler.remove(fiber);

    assertEquals(0, fiberCounter.get(), "FiberScheduler triggered early");

    System.out.println("Run 2");
    fiberCounter.set(0);
    scheduler.add(fiber);
    try {
      Thread.sleep(600);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    scheduler.remove(fiber);

    assertEquals(1, fiberCounter.get(),
        "FiberScheduler either didn't trigger or triggered more than once");

    // Run 3
    fiberCounter.set(0);
    scheduler.add(fiber);
    try {
      Thread.sleep(1000);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    scheduler.remove(fiber);

    // Fiber scheduled every 400ms runs twice in 1000ms
    assertEquals(2, fiberCounter.get(),
        "FiberScheduler either didn't trigger or triggered more than once");
  }

  @Test
  void multiFiberTest() {
    final AtomicInteger fiberCounter1 = new AtomicInteger(0);
    final AtomicInteger fiberCounter2 = new AtomicInteger(0);

    final FiberScheduler scheduler = new FiberScheduler();
    final Fiber fiber1 = new Fiber(0.2, () -> {
      fiberCounter1.addAndGet(1);
    });
    final Fiber fiber2 = new Fiber(0.6, () -> {
      fiberCounter2.addAndGet(1);
    });

    scheduler.add(fiber2);
    try {
      Thread.sleep(200);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    assertEquals(0, fiberCounter1.get(), "FiberScheduler triggered early");
    assertEquals(0, fiberCounter2.get(), "FiberScheduler triggered early");

    // Sleep enough such that only the fiber enabled later times out first
    scheduler.add(fiber1);
    try {
      Thread.sleep(300);
    } catch (InterruptedException ex) {
      Thread.currentThread().interrupt();
    }
    scheduler.remove(fiber1);
    scheduler.remove(fiber2);

    assertEquals(1, fiberCounter1.get(),
        "FiberScheduler either didn't trigger or triggered more than once");
    assertEquals(0, fiberCounter2.get(), "FiberScheduler triggered early");
  }
}
