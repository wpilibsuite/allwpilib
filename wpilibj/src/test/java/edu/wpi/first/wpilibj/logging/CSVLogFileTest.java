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
import java.nio.file.Paths;
import java.util.Scanner;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;

class CSVLogFileTest {
  @Test
  void logsTest() {
    CSVLogFile csvLogFile = new CSVLogFile("testCSVLog", "First column", 2);
    csvLogFile.log(1, "Escaping \"double\" quotes.");

    try {
      Scanner testFileScanner = new Scanner(new File(csvLogFile.getFileName()));
      assertEquals("\"Timestamp (ms)\",\"First column\",2", testFileScanner.nextLine());

      String secondLine = testFileScanner.nextLine();
      String timestamp = secondLine.substring(0, secondLine.indexOf(','));
      assertEquals(",1,\"Escaping \"\"double\"\" quotes.\"", secondLine.replace(timestamp, ""));

      testFileScanner.close();
    } catch (IOException ex) {
      ex.printStackTrace();
    }

    try {
      Files.delete(Paths.get(csvLogFile.getFileName()));
    } catch (IOException ex) {
      ex.printStackTrace();
    }
  }
}
