// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands2;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.wpilib.hardware.hal.HAL;
import org.wpilib.simulation.SimHooks;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class NotifierCommandTest extends CommandTestBase {
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
  void notifierCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger(0);

      NotifierCommand command = new NotifierCommand(counter::incrementAndGet, 0.01);

      scheduler.schedule(command);
      for (int i = 0; i < 5; ++i) {
        SimHooks.stepTiming(0.005);
      }
      scheduler.cancel(command);

      assertEquals(2, counter.get());
    }
  }
}
