// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.io.IOException;

/** A data log writer that flushes the data log to a file when flush() is called. */
public class DataLogWriter extends DataLog {
  /**
   * Construct a new Data Log.
   *
   * @param filename filename to use
   * @param extraHeader extra header data
   * @throws IOException if file cannot be opened
   */
  public DataLogWriter(String filename, String extraHeader) throws IOException {
    super(DataLogJNI.fgCreate(filename, extraHeader));
  }

  /**
   * Construct a new Data Log.
   *
   * @param filename filename to use
   * @throws IOException if file cannot be opened
   */
  public DataLogWriter(String filename) throws IOException {
    this(filename, "");
  }
}
