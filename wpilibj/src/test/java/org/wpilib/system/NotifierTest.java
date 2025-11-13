// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;
import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.SimHooks;

/** Tests to see if the Notifier is working properly. */
class NotifierTest {
  @BeforeEach
  void setup() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    SimHooks.restartTiming();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  @Test
  @ResourceLock("timing")
  void testStartPeriodicAndStop() {
    AtomicInteger counter = new AtomicInteger();
    Notifier notifier = new Notifier(counter::getAndIncrement);
    notifier.startPeriodic(1.0);

    SimHooks.stepTiming(10);

    notifier.stop();
    assertEquals(10, counter.get());

    SimHooks.stepTiming(3.0);

    assertEquals(10, counter.get());

    notifier.close();
  }

  @Test
  @ResourceLock("timing")
  void testStartSingle() {
    AtomicInteger counter = new AtomicInteger();
    Notifier notifier = new Notifier(counter::getAndIncrement);
    notifier.startSingle(1.0);

    SimHooks.stepTiming(10.5);

    assertEquals(1, counter.get());

    notifier.close();
  }
}
