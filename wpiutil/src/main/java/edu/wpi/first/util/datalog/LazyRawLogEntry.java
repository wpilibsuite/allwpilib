// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.util.Arrays;

/**
 * Lazy log raw byte array values. This will only write if the value has been changed since last
 * append call
 */
public class LazyRawLogEntry extends RawLogEntry {
  private byte[] m_lastValue;
  private boolean m_firstAppend = true;

  public LazyRawLogEntry(DataLog log, String name, String metadata, String type, long timestamp) {
    super(log, name, metadata, type, timestamp);
  }

  public LazyRawLogEntry(DataLog log, String name, String metadata, String type) {
    super(log, name, metadata, type);
  }

  public LazyRawLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, metadata, timestamp);
  }

  public LazyRawLogEntry(DataLog log, String name, String metadata) {
    super(log, name, metadata);
  }

  public LazyRawLogEntry(DataLog log, String name, long timestamp) {
    super(log, name, timestamp);
  }

  public LazyRawLogEntry(DataLog log, String name) {
    super(log, name);
  }

  @Override
  public void append(byte[] value, long timestamp) {
    if (m_firstAppend || !Arrays.equals(value, m_lastValue)) {
      m_firstAppend = false;
      m_lastValue = Arrays.copyOf(value, value.length);
      super.append(value, timestamp);
    }
  }
}
