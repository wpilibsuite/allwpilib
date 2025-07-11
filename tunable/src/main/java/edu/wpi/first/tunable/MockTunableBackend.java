// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.tunable;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.function.Consumer;
import java.util.function.IntConsumer;

public class MockTunableBackend implements TunableBackend {

  private final Map<String, Object> m_entries = new HashMap<>();
  private final Set<Runnable> m_changed = new HashSet<>();

  private final class MockIntTunable implements IntTunable, Runnable {
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

  private static class MockTunable<T> implements Tunable<T> {
    @Override
    public void setDefault(T value);

    @Override
    public void set(T value);

    @Override
    public T get();

    @Override
    public void onChange(Consumer<T> callback);
  }

  public void set(String name, int value);
  public <T> void set(String name, T value);

  @Override
  public IntTunable addInt(String name, int defaultValue) {
    return new MockIntTunable(defaultValue);
  }

  @Override
  public <T> Tunable<T> addObject(String name, T defaultValue) {
    return new MockTunable()
  }

  @Override
  public <T> Tunable<T> addStruct(String name, T defaultValue, Struct<T> struct);

  @Override
  public <T> Tunable<T> addProtobuf(String name, T defaultValue, Protobuf<T, ?> proto);

  @Override
  public void update() {

  }
}
