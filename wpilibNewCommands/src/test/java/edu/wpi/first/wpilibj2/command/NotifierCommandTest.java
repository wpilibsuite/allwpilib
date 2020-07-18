/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.SimHooks;

import static org.junit.jupiter.api.Assertions.assertEquals;

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
      Counter counter = new Counter();

      NotifierCommand command = new NotifierCommand(counter::increment, 0.01);

      scheduler.schedule(command);
      for (int i = 0; i < 5; ++i) {
        SimHooks.stepTiming(0.005);
      }
      scheduler.cancel(command);

      assertEquals(2, counter.m_counter);
    }
  }
}
