// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;
import org.junit.jupiter.api.Test;

class PrintCommandTest extends CommandTestBase {
  @Test
  void printCommandScheduleTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      final PrintStream originalOut = System.out;
      ByteArrayOutputStream testOut = new ByteArrayOutputStream();
      System.setOut(new PrintStream(testOut));
      try {
        PrintCommand command = new PrintCommand("Test!");

        scheduler.schedule(command);
        scheduler.run();

        assertFalse(scheduler.isScheduled(command));
        assertEquals(testOut.toString(), "Test!" + System.lineSeparator());
      } finally {
        System.setOut(originalOut);
      }
    }
  }
}
