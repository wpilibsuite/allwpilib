// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util;

import edu.wpi.first.util.datalog.DataLog;

/**
 * A class version of `tail -f`, otherwise known as `tail -f` at home. Watches a file and puts the
 * data into a data log. Only works on Linux-based platforms.
 */
public class FileLogger implements AutoCloseable {
  private final long m_impl;

  /**
   * Construct a FileLogger. When the specified file is modified, appended data will be appended to
   * the specified data log.
   *
   * @param file The path to the file.
   * @param log A data log.
   * @param key The log key to append data to.
   */
  public FileLogger(String file, DataLog log, String key) {
    m_impl = WPIUtilJNI.createFileLogger(file, log.getImpl(), key);
  }

  @Override
  public void close() {
    WPIUtilJNI.freeFileLogger(m_impl);
  }
}
