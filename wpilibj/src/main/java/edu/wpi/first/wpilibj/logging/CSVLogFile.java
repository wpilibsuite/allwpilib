/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

import java.util.Arrays;

/**
 * A CSVLogFile writes to a csv file
 *
 * <p>For the CSVLogFile to write log informations, you must call
 * {@link #log} periodically.
 */
public class CSVLogFile {
  private final LogFile m_logFile;

  /**
   * Instantiate a LogFile passing in its prefix and its extension.
   *
   * <p>If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on
   * the roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix The prefix of the LogFile.
   */
  public CSVLogFile(String filePrefix, String... columnHeadings) {
    m_logFile = new LogFile(filePrefix, "csv");

    m_logFile.log("Timestamp (ms),");
    logValues(columnHeadings);
  }

  public <ValueT> void log(ValueT... values) {
    m_logFile.log(System.currentTimeMillis() + ",");
    logValues(values);
  }

  private <ValueT> void logValues(ValueT... values) {
    if (values.length > 1) {
      m_logFile.log(values[0] + ",");
      logValues(Arrays.copyOfRange(values, 1, values.length));
    } else {
      System.out.println("end");
      m_logFile.log(values[0] + "\n");
    }
  }
}
