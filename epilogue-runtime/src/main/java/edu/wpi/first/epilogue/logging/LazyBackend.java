// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.util.struct.Struct;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * A backend implementation that only logs data when it changes. Useful for keeping bandwidth and
 * file sizes down. However, because it still needs to check that data has changed, it cannot avoid
 * expensive sensor reads.
 */
public class LazyBackend implements EpilogueBackend {
  private final EpilogueBackend m_backend;

  // Keep a record of the most recent value written to each entry
  // Note that this may duplicate a lot of data, and will box primitives.
  private final Map<String, Object> m_previousValues = new HashMap<>();
  private final Map<String, NestedBackend> m_subLoggers = new HashMap<>();

  /**
   * Creates a new lazy backend wrapper around another backend.
   *
   * @param backend the backend to delegate to
   */
  public LazyBackend(EpilogueBackend backend) {
    this.m_backend = backend;
  }

  @Override
  public EpilogueBackend lazy() {
    // Already lazy, don't need to wrap it again
    return this;
  }

  @Override
  public EpilogueBackend getNested(String path) {
    return m_subLoggers.computeIfAbsent(path, k -> new NestedBackend(k, this));
  }

  @Override
  public void log(String identifier, int value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof Integer oldValue && oldValue == value) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value);
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, long value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof Long oldValue && oldValue == value) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value);
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, float value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof Float oldValue && oldValue == value) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value);
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, double value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof Double oldValue && oldValue == value) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value);
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, boolean value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof Boolean oldValue && oldValue == value) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value);
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, byte[] value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof byte[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, int[] value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof int[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, long[] value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof long[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, float[] value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof float[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, double[] value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof double[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, boolean[] value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof boolean[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, String value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof String oldValue && oldValue.equals(value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value);
    m_backend.log(identifier, value);
  }

  @Override
  public void log(String identifier, String[] value) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof String[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value);
  }

  @Override
  public <S> void log(String identifier, S value, Struct<S> struct) {
    var previous = m_previousValues.get(identifier);

    if (Objects.equals(previous, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value);
    m_backend.log(identifier, value, struct);
  }

  @Override
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    var previous = m_previousValues.get(identifier);

    if (previous instanceof Object[] oldValue && Arrays.equals(oldValue, value)) {
      // no change
      return;
    }

    m_previousValues.put(identifier, value.clone());
    m_backend.log(identifier, value, struct);
  }
}
