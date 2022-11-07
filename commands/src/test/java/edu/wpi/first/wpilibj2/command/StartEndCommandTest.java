// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;

class StartEndCommandTest extends CommandTestBase {
  @Test
  void startEndCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean cond1 = new AtomicBoolean();
      AtomicBoolean cond2 = new AtomicBoolean();

      StartEndCommand command = new StartEndCommand(() -> cond1.set(true), () -> cond2.set(true));

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(scheduler.isScheduled(command));

      scheduler.cancel(command);

      assertFalse(scheduler.isScheduled(command));
      assertTrue(cond1.get());
      assertTrue(cond2.get());
    }
  }
}
