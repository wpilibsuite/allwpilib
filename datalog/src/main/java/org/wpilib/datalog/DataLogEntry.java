// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

/** Log entry base class. */
public class DataLogEntry {
  /**
   * Constructs a data log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param type Data type
   * @param metadata metadata
   * @param timestamp entry creation timestamp (0=now)
   */
  protected DataLogEntry(DataLog log, String name, String type, String metadata, long timestamp) {
    m_log = log;
    m_entry = log.start(name, type, metadata, timestamp);
  }

  /**
   * Constructs a data log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param type Data type
   * @param metadata metadata
   */
  protected DataLogEntry(DataLog log, String name, String type, String metadata) {
    this(log, name, type, metadata, 0);
  }

  /**
   * Constructs a data log entry.
   *
   * @param log datalog
   * @param name name of the entry
   * @param type Data type
   */
  protected DataLogEntry(DataLog log, String name, String type) {
    this(log, name, type, "");
  }

  /**
   * Updates the metadata for the entry.
   *
   * @param metadata New metadata for the entry
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void setMetadata(String metadata, long timestamp) {
    m_log.setMetadata(m_entry, metadata, timestamp);
  }

  /**
   * Updates the metadata for the entry.
   *
   * @param metadata New metadata for the entry
   */
  public void setMetadata(String metadata) {
    setMetadata(metadata, 0);
  }

  /**
   * Finishes the entry.
   *
   * @param timestamp Time stamp (0 to indicate now)
   */
  public void finish(long timestamp) {
    m_log.finish(m_entry, timestamp);
  }

  /** Finishes the entry. */
  public void finish() {
    finish(0);
  }

  /** The data log instance associated with the entry. */
  protected final DataLog m_log;

  /** The data log entry index. */
  protected final int m_entry;
}
