// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.mockito.ArgumentMatchers.anyDouble;
import static org.mockito.ArgumentMatchers.notNull;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;
import static org.mockito.Mockito.when;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class WaitCommandTest extends CommandTestBase {
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
  void waitCommandTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      WaitCommand waitCommand = new WaitCommand(2);

      scheduler.schedule(waitCommand);
      scheduler.run();
      SimHooks.stepTiming(1);
      scheduler.run();

      assertTrue(scheduler.isScheduled(waitCommand));

      SimHooks.stepTiming(2);

      scheduler.run();

      assertFalse(scheduler.isScheduled(waitCommand));
    }
  }

  @Test
  @ResourceLock("timing")
  void withTimeoutTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      when(command1.withTimeout(anyDouble())).thenCallRealMethod();
      when(command1.raceWith(notNull())).thenCallRealMethod();

      Command timeout = command1.withTimeout(2);

      scheduler.schedule(timeout);
      scheduler.run();

      verify(command1).initialize();
      verify(command1).execute();
      assertFalse(scheduler.isScheduled(command1));
      assertTrue(scheduler.isScheduled(timeout));

      SimHooks.stepTiming(3);
      scheduler.run();

      verify(command1).end(true);
      verify(command1, never()).end(false);
      assertFalse(scheduler.isScheduled(timeout));
    }
  }
}
