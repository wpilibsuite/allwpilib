/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

import java.util.Arrays;

/**
 * A CSVLogFile writes values to a csv file
 *
 * <p>For the CSVLogFile to write log informations, you must call {@link #log}
 * periodically.
 */
public class CSVLogFile {
  private final LogFile m_logFile;

  /**
   * Instantiate a LogFile passing in its prefix and its column headings.
   *
   * <p>If you want the file to be saved in a existing directory, you can add its
   * path before the file prefix. Exemple : to save the file in a usb stick on the
   * roborio ("/media/sda1/") : LogFile("/media/sda1/log").
   *
   * @param filePrefix     The prefix of the LogFile.
   * @param columnHeadings Titles of CSVLogFile columns.
   */
  public <ValueT> CSVLogFile(String filePrefix, ValueT... columnHeadings) {
    m_logFile = new LogFile(filePrefix, "csv");

    m_logFile.log("\"Timestamp (ms)\",");
    logValues(columnHeadings);
  }

  /**
   * Print a new line of values in the CSVLogFile.
   *
   * @param values Values to log in the file in the order.
   */
  public <ValueT> void log(ValueT... values) {
    m_logFile.log(System.currentTimeMillis() + ",");
    logValues(values);
  }

  /**
   * Get the name the file.
   *
   * @return The name of the file.
   */
  public String getFileName() {
    return m_logFile.getFileName();
  }

  /**
   * Print a new line of values in the CSVLogFile without timestamp.
   *
   * @param values Values to log in the file in the order.
   */
  private <ValueT> void logValues(ValueT... values) {
    if (String.class.isAssignableFrom(values[0].getClass())) {
      m_logFile.log("\"" + escapeDoubleQuotes(values[0].toString()) + "\"");
    } else {
      m_logFile.log(String.valueOf(values[0]));
    }

    if (values.length > 1) {
      m_logFile.log(",");
      logValues(Arrays.copyOfRange(values, 1, values.length));
    } else {
      m_logFile.log("\n");
    }
  }

  /**
   * Escape double quotes in a text by duplicating them.
   *
   * @param text Text to escape.
   * @return The text with all its double quotes escaped.
   */
  private String escapeDoubleQuotes(String text) {
    return text.replace("\"", "\"\"");
  }
}
