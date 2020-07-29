/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

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
