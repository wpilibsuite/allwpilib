/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

/**
 * Represents a column in a CSVLogFile.
 *
 * <p>For the column to be printed, you must register the instance of a CSVLogCell
 * in a CSVLogFile using {@link CSVLogFile#registerCell(CSVLogCell cell)}.
 */
public class CSVLogCell {
  private final String m_name;
  private String m_content;

  /**
   * Instantiate a CSVLogCell passing in the name of the column.
   *
   * @param name The name of the column.
   */
  public CSVLogCell(String name) {
    if (isStringValid(name)) {
      m_name = name;
    } else {
      m_name = "";
    }
  }

  /**
   * Log any type of value that can be converted into a string.
   *
   * @param value The value to be logged in the cell.
   */
  public <T> void log(T value) {
    if (isStringValid(String.valueOf(value))) {
      m_content = String.valueOf(value);
    }
  }

  /**
   * Gets the name of the cell.
   *
   * @return The name of the cell.
   */
  public String getName() {
    return m_name;
  }

  /**
   * Get the contents of the string.
   *
   * @return The content.
   */
  public String getContent() {
    return m_content;
  }

  /**
   * Clear the cell so its content is empty.
   */
  public void clearCell() {
    m_content = "";
  }

  private boolean isStringValid(String string) {
    if (string.contains("\"")) {
      System.out.println("CSVLogCell's name/content cannot contain double quotes");
      return false;
    } else if (string.contains("\n")) {
      System.out.println("CSVLogCell's name/content cannot contain newlines");
      return false;
    }
    return true;
  }
}
