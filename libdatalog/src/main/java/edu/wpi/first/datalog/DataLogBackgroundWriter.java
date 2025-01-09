// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/**
 * A data log background writer that periodically flushes the data log on a background thread. The
 * data log file is created immediately upon construction with a temporary filename. The file may be
 * renamed at any time using the setFilename() function.
 *
 * <p>The data log is periodically flushed to disk. It can also be explicitly flushed to disk by
 * using the flush() function. This operation is, however, non-blocking.
 */
public final class DataLogBackgroundWriter extends DataLog {
  /**
   * Construct a new Data Log. The log will be initially created with a temporary filename.
   *
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is generated of the form
   *     "wpilog_{}.wpilog"
   * @param period time between automatic flushes to disk, in seconds; this is a time/storage
   *     tradeoff
   * @param extraHeader extra header data
   */
  public DataLogBackgroundWriter(String dir, String filename, double period, String extraHeader) {
    super(DataLogJNI.bgCreate(dir, filename, period, extraHeader));
  }

  /**
   * Construct a new Data Log. The log will be initially created with a temporary filename.
   *
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is generated of the form
   *     "wpilog_{}.wpilog"
   * @param period time between automatic flushes to disk, in seconds; this is a time/storage
   *     tradeoff
   */
  public DataLogBackgroundWriter(String dir, String filename, double period) {
    this(dir, filename, period, "");
  }

  /**
   * Construct a new Data Log. The log will be initially created with a temporary filename.
   *
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is generated of the form
   *     "wpilog_{}.wpilog"
   */
  public DataLogBackgroundWriter(String dir, String filename) {
    this(dir, filename, 0.25);
  }

  /**
   * Construct a new Data Log. The log will be initially created with a temporary filename.
   *
   * @param dir directory to store the log
   */
  public DataLogBackgroundWriter(String dir) {
    this(dir, "", 0.25);
  }

  /** Construct a new Data Log. The log will be initially created with a temporary filename. */
  public DataLogBackgroundWriter() {
    this("");
  }

  /**
   * Change log filename.
   *
   * @param filename filename
   */
  public void setFilename(String filename) {
    DataLogJNI.bgSetFilename(m_impl, filename);
  }

  /**
   * Resumes appending of data records to the log. If called after stop(), opens a new file (with
   * random name if SetFilename was not called after stop()) and appends Start records and schema
   * data values for all previously started entries and schemas.
   */
  @Override
  public void resume() {
    DataLogJNI.resume(m_impl);
  }
}
