/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

import java.io.File;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.Scanner;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

class LogFileTest {
  @Test
  void logsTest() {
    LogFile logFile = new LogFile("testLog");
    logFile.log("First line\n");
    logFile.logln("Second line");

    try {
      Scanner testFileScanner = new Scanner(new File(logFile.getFileName()));
      assertEquals("First line", testFileScanner.nextLine());
      assertEquals("Second line", testFileScanner.nextLine());
    } catch (IOException ex) {
      ex.printStackTrace();
    }
  }

  @Test
  void filename() {
    String filenamePrefix = "testFilename";
    LogFile logFile = new LogFile(filenamePrefix);
    logFile.log("");

    SimpleDateFormat formater = new SimpleDateFormat("yyyy-MM-dd.HH_mm_ss", Locale.getDefault());
    Path filePath = Paths.get(filenamePrefix + "-" + formater.format(new Date()) + ".txt");

    assertTrue(Files.exists(filePath));
  }
}
