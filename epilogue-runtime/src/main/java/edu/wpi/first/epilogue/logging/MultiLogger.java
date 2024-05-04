// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * A data logger implementation that delegates to other loggers. Helpful for simultaneous logging to
 * multiple data stores at once.
 */
public class MultiLogger implements DataLogger {
  private final List<DataLogger> m_loggers;
  private final Map<String, SubLogger> m_subLoggers = new HashMap<>();

  // Use DataLogger.multi(...) instead of instantiation directly
  MultiLogger(DataLogger... loggers) {
    this.m_loggers = List.of(loggers);
  }

  @Override
  public DataLogger getSubLogger(String path) {
    return m_subLoggers.computeIfAbsent(path, k -> new SubLogger(k, this));
  }

  @Override
  public void log(String identifier, int value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, long value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, float value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, double value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, boolean value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, byte[] value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, int[] value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, long[] value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, float[] value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, double[] value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, boolean[] value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, String value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public void log(String identifier, String[] value) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value);
    }
  }

  @Override
  public <S> void log(String identifier, S value, Struct<S> struct) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value, struct);
    }
  }

  @Override
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    for (DataLogger logger : m_loggers) {
      logger.log(identifier, value, struct);
    }
  }
}
