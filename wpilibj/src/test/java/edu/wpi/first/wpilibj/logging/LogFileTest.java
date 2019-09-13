/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

import java.io.File;
import java.io.IOException;
import java.util.Scanner;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class LogFileTest {
  @Test
  void writeLogTest() {
    LogFile logFile = new LogFile("logfile");
    logFile.log("First line\n");
    logFile.logln("Second line");

    StringBuilder line = new StringBuilder();
    try {
      Scanner testFileScanner = new Scanner(new File(logFile.getFileName()));
      while (testFileScanner.hasNextLine()) {
        line.append(testFileScanner.nextLine());
      }
    } catch (IOException ex) {
      ex.printStackTrace();
    }

    assertEquals("First lineSecond line", line);
  }
}
