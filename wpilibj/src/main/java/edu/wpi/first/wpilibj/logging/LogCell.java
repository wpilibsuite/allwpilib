/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.logging;

import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

/**
 * Represents a column in a LogSpreadsheet.
 *
 * <p>For the column to be printed, you must register the instance of a LogCell
 * in a LogSpreadsheet using {@link LogSpreadsheet#registerCell(LogCell cell)}.
 */
public class LogCell {
  private final String m_name;
  private final Lock m_mutex;
  private String m_content;

  /**
   * Instantiate a LogCell passing in the name of the column.
   *
   * @param name The name of the column.
   */
  public LogCell(String name) {
    m_name = name;
    m_mutex = new ReentrantLock();
  }

  /**
   * Log any type of value that can be converted into a string.
   *
   * @param value The value to be logged in the cell.
   */
  public <T> void log(T value) {
    acquireLock();
    m_content = String.valueOf(value);
    releaseLock();
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

  /**
   * Acquire the lock on the cell's content.
   */
  public void acquireLock() {
    m_mutex.lock();
  }

  /**
   * Release the lock on the cell's content.
   */
  public void releaseLock() {
    m_mutex.unlock();
  }
}
