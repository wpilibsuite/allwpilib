// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class RunCommandTest extends CommandTestBase {
  @Test
  void runCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      AtomicInteger counter = new AtomicInteger(0);

      RunCommand command = new RunCommand(counter::incrementAndGet);

      scheduler.schedule(command);
      scheduler.run();
      scheduler.run();
      scheduler.run();

      assertEquals(3, counter.get());
    }
  }
}
