// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

/** Log entry base class. */
public class DataLogEntry {
  protected DataLogEntry(DataLog log, String name, String type, String metadata, long timestamp) {
    m_log = log;
    m_entry = log.start(name, type, metadata, timestamp);
  }

  protected DataLogEntry(DataLog log, String name, String type, String metadata) {
    this(log, name, type, metadata, 0);
  }

  protected DataLogEntry(DataLog log, String name, String type) {
    this(log, name, type, "");
  }

  /**
   * Updates the metadata for the entry.
   *
   * @param metadata New metadata for the entry
   * @param timestamp Time stamp (may be 0 to indicate now)
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
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  public void finish(long timestamp) {
    m_log.finish(m_entry, timestamp);
  }

  /** Finishes the entry. */
  public void finish() {
    finish(0);
  }

  protected final DataLog m_log;
  protected final int m_entry;
}
