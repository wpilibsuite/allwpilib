// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import org.junit.jupiter.api.Test;

class InstantCommandTest extends CommandTestBase {
  @Test
  void instantCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicBoolean cond = new AtomicBoolean();

      InstantCommand command = new InstantCommand(() -> cond.set(true));

      scheduler.schedule(command);
      scheduler.run();

      assertTrue(cond.get());
      assertFalse(scheduler.isScheduled(command));
    }
  }
}
