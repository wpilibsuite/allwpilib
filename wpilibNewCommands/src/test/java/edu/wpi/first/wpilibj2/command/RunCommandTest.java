// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class RunCommandTest extends CommandTestBase {
  @Test
  void runCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      Counter counter = new Counter();

      RunCommand command = new RunCommand(counter::increment);

      scheduler.schedule(command);
      scheduler.run();
      scheduler.run();
      scheduler.run();

      assertEquals(3, counter.m_counter);
    }
  }
}
