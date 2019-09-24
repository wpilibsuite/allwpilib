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
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

class LogFileTest {
  @Test
  void logsTest() {
    LogFile logFile = new LogFile("testLog");
    logFile.log("First line\n");
    logFile.logln("Second line");

    try {
      logFile.write("-----Third line-----", 5, 10);
    } catch (IOException ex) {
      System.out.println(ex.getMessage());
    }

    try {
      Scanner testFileScanner = new Scanner(new File(logFile.getFileName()));
      assertEquals("First line", testFileScanner.nextLine());
      assertEquals("Second line", testFileScanner.nextLine());
      assertEquals("Third line", testFileScanner.nextLine());
      testFileScanner.close();
    } catch (IOException ex) {
      ex.printStackTrace();
    }

    try {
      Files.delete(Paths.get(logFile.getFileName()));
    } catch (IOException ex) {
      ex.printStackTrace();
    }
  }

  @Test
  void filename() {
    String filenamePrefix = "testFilename";

    LogFile logFile = new LogFile(filenamePrefix);
    logFile.setTimeIntervalBeforeRenaming(1);
    logFile.log("");

    Path firstFilePath = getExpectedFilename(filenamePrefix);
    assertTrue(Files.exists(firstFilePath));

    try {
      Thread.sleep(1100);
    } catch (InterruptedException ex) {
      ex.printStackTrace();
    }
    logFile.log("");

    Path filePath = getExpectedFilename(filenamePrefix);
    assertFalse(Files.exists(firstFilePath));
    assertTrue(Files.exists(filePath));

    try {
      Files.delete(filePath);
    } catch (IOException ex) {
      ex.printStackTrace();
    }
  }

  Path getExpectedFilename(String filenamePrefix) {
    SimpleDateFormat formater = new SimpleDateFormat("yyyy-MM-dd.HH_mm_ss", Locale.getDefault());
    return Paths.get(filenamePrefix + "-" + formater.format(new Date()) + ".txt");
  }
}
