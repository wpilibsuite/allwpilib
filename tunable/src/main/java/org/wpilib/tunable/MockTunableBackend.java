// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.function.Consumer;
import java.util.function.IntConsumer;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

public class MockTunableBackend implements TunableBackend {

  private final Map<String, Object> m_entries = new HashMap<>();
  private final Set<Runnable> m_changed = new HashSet<>();

  private final class MockIntTunable implements TunableInt, Runnable {
    MockIntTunable(int defaultValue) {
      m_defaultValue = defaultValue;
    }

    @Override
    public void setDefault(int value) {
      m_defaultValue = value;
    }

    @Override
    public void set(int value) {
      synchronized (this) {
        if (!m_hasValue || m_value != value) {
          m_changed.add(this);
        }
        m_value = value;
        m_hasValue = true;
      }
    }

    @Override
    public int get() {
      synchronized (this) {
        if (m_hasValue) {
          return m_value;
        } else {
          return m_defaultValue;
        }
      }
    }

    @Override
    public void onChange(IntConsumer callback) {
      synchronized (this) {
        m_onChange = callback;
      }
    }

    @Override
    public void run() {
      synchronized (this) {
        if (m_onChange != null) {
          m_onChange.accept(m_value);
        }
      }
    }

    private int m_defaultValue;
    private int m_value;
    private boolean m_hasValue;
    private IntConsumer m_onChange;
  }

  private final class MockTunable<T> implements Tunable<T>, Runnable {
    MockTunable(T defaultValue) {
      m_defaultValue = defaultValue;
    }

    @Override
    public void setDefault(T value) {
      m_defaultValue = value;
    }

    @Override
    public void set(T value) {
      synchronized (this) {
        if (!m_hasValue || !m_value.equals(value)) {
          m_changed.add(this);
        }
        m_value = value;
        m_hasValue = true;
      }
    }

    @Override
    public T get() {
      synchronized (this) {
        if (m_hasValue) {
          return m_value;
        } else {
          return m_defaultValue;
        }
      }
    }

    @Override
    public void onChange(Consumer<T> callback) {
      synchronized (this) {
        m_onChange = callback;
      }
    }

    @Override
    public void run() {
      synchronized (this) {
        if (m_onChange != null) {
          m_onChange.accept(m_value);
        }
      }
    }

    private T m_defaultValue;
    private T m_value;
    private boolean m_hasValue;
    private Consumer<T> m_onChange;
  }

  /**
   * Sets the value of a tunable.
   *
   * @param name tunable name
   * @param value value
   */
  public void set(String name, int value) {
    synchronized (this) {
      TunableInt tunable = (TunableInt) m_entries.get(name);
      if (tunable == null) {
        throw new IllegalArgumentException("No such tunable: " + name);
      }
      tunable.set(value);
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param name tunable name
   * @param value value
   */
  public <T> void set(String name, T value) {
    synchronized (this) {
      @SuppressWarnings("unchecked")
      Tunable<T> tunable = (Tunable<T>) m_entries.get(name);
      if (tunable == null) {
        throw new IllegalArgumentException("No such tunable: " + name);
      }
      tunable.set(value);
    }
  }

  @Override
  public TunableInt addInt(String name, int defaultValue) {
    synchronized (this) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      TunableInt tunable = new MockIntTunable(defaultValue);
      m_entries.put(name, tunable);
      return tunable;
    }
  }

  @Override
  public <T> Tunable<T> addObject(String name, T defaultValue) {
    synchronized (this) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      Tunable<T> tunable = new MockTunable<T>(defaultValue);
      m_entries.put(name, tunable);
      return tunable;
    }
  }

  @Override
  public <T> Tunable<T> addStruct(String name, T defaultValue, Struct<T> struct) {
    return addObject(name, defaultValue);
  }

  @Override
  public <T> Tunable<T> addProtobuf(String name, T defaultValue, Protobuf<T, ?> proto) {
    return addObject(name, defaultValue);
  }

  @Override
  public void update() {}
}
