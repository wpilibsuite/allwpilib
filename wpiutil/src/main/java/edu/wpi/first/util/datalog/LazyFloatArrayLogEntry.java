// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.util.Arrays;

/**
 * Lazy log array of float values. This will only write if the value has been changed since last
 * append call
 */
public class LazyFloatArrayLogEntry extends FloatArrayLogEntry {
  private float[] m_lastValue;
  private boolean m_firstAppend = true;

  public LazyFloatArrayLogEntry(DataLog log, String name, String metadata, long timestamp) {
    super(log, name, metadata, timestamp);
  }

  public LazyFloatArrayLogEntry(DataLog log, String name, String metadata) {
    super(log, name, metadata);
  }

  public LazyFloatArrayLogEntry(DataLog log, String name, long timestamp) {
    super(log, name, timestamp);
  }

  public LazyFloatArrayLogEntry(DataLog log, String name) {
    super(log, name);
  }

  @Override
  public void append(float[] value, long timestamp) {
    if (m_firstAppend || !Arrays.equals(value, m_lastValue)) {
      m_firstAppend = false;
      m_lastValue = Arrays.copyOf(value, value.length);
      super.append(value, timestamp);
    }
  }
}
