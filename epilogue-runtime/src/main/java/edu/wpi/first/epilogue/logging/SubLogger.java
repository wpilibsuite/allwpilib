// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.Map;

/**
 * A data logger that logs to an underlying logger, prepending all logged data with a specific
 * prefix. Useful for logging nested data structures.
 */
public class SubLogger implements DataLogger {
  private final String m_prefix;
  private final DataLogger m_impl;
  private final Map<String, SubLogger> m_subLoggers = new HashMap<>();

  /**
   * Creates a new sublogger underneath another logger.
   *
   * @param prefix the prefix to append to all data logged in the sublogger
   * @param impl the data logger to log to
   */
  public SubLogger(String prefix, DataLogger impl) {
    // Add a trailing slash if not already present
    if (prefix.endsWith("/")) {
      this.m_prefix = prefix;
    } else {
      this.m_prefix = prefix + "/";
    }
    this.m_impl = impl;
  }

  @Override
  public DataLogger getSubLogger(String path) {
    return m_subLoggers.computeIfAbsent(path, k -> new SubLogger(k, this));
  }

  @Override
  public void log(String identifier, int value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, long value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, float value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, double value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, boolean value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, byte[] value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, int[] value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, long[] value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, float[] value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, double[] value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, boolean[] value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, String value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public void log(String identifier, String[] value) {
    m_impl.log(m_prefix + identifier, value);
  }

  @Override
  public <S> void log(String identifier, S value, Struct<S> struct) {
    m_impl.log(m_prefix + identifier, value, struct);
  }

  @Override
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    m_impl.log(m_prefix + identifier, value, struct);
  }
}
