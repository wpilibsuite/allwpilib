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
    WaitCommand waitCommand = new WaitCommand(2);

    waitCommand.schedule();
    CommandScheduler.getInstance().run();
    SimHooks.stepTiming(1);
    CommandScheduler.getInstance().run();

    assertTrue(waitCommand.isScheduled());

    SimHooks.stepTiming(2);

    CommandScheduler.getInstance().run();

    assertFalse(waitCommand.isScheduled());
  }

  @Test
  @ResourceLock("timing")
  void withTimeoutTest() {
    MockCommandHolder command1Holder = new MockCommandHolder(true);
    Command command1 = command1Holder.getMock();
    when(command1.withTimeout(anyDouble())).thenCallRealMethod();
    when(command1.raceWith(notNull())).thenCallRealMethod();

    Command timeout = command1.withTimeout(2);

    timeout.schedule();
    CommandScheduler.getInstance().run();

    verify(command1).initialize();
    verify(command1).execute();
    assertFalse(command1.isScheduled());
    assertTrue(timeout.isScheduled());

    SimHooks.stepTiming(3);
    CommandScheduler.getInstance().run();

    verify(command1).end(true);
    verify(command1, never()).end(false);
    assertFalse(timeout.isScheduled());
  }
}
