// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/** A mock tunable backend for testing. */
public class MockTunableBackend implements TunableBackend {
  private final Map<String, Object> m_entries = new HashMap<>();

  private record Action(String path, Runnable update) {}

  private final List<Action> m_actions = new ArrayList<>();

  @Override
  public void close() {
    synchronized (m_entries) {
      m_entries.clear();
    }
  }

  private static void notifyOnTune(TunableBase tunable) {
    tunable.resetChanged();
    var config = tunable.getConfig();
    if (config != null) {
      var onTune = config.getOnTune();
      if (onTune != null) {
        onTune.run();
      }
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public boolean getBoolean(String path) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableBoolean tunable) {
        return tunable.get();
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableBoolean");
      }
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public int getInteger(String path) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableInt tunable) {
        return tunable.get();
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableInt");
      }
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public long getLong(String path) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableLong tunable) {
        return tunable.get();
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableLong");
      }
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public float getFloat(String path) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableFloat tunable) {
        return tunable.get();
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableFloat");
      }
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public double getDouble(String path) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableDouble tunable) {
        return tunable.get();
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableDouble");
      }
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public <T> T getValue(String path) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable) {
        @SuppressWarnings("unchecked")
        Tunable<T> typedTunable = (Tunable<T>) tunable;
        return typedTunable.get();
      } else {
        throw new IllegalArgumentException(
            "Tunable at " + path + " is not a Tunable of the expected type");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setBoolean(String path, boolean value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableBoolean tunable) {
        m_actions.add(
            new Action(
                path,
                () -> {
                  tunable.set(value);
                  notifyOnTune(tunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableBoolean");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setInt(String path, int value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableInt tunable) {
        m_actions.add(
            new Action(
                path,
                () -> {
                  tunable.set(value);
                  notifyOnTune(tunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableInt");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setLong(String path, long value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableLong tunable) {
        m_actions.add(
            new Action(
                path,
                () -> {
                  tunable.set(value);
                  notifyOnTune(tunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableLong");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setFloat(String path, float value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableFloat tunable) {
        m_actions.add(
            new Action(
                path,
                () -> {
                  tunable.set(value);
                  notifyOnTune(tunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableFloat");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setDouble(String path, double value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof TunableDouble tunable) {
        m_actions.add(
            new Action(
                path,
                () -> {
                  tunable.set(value);
                  notifyOnTune(tunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a TunableDouble");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public <T> void setValue(String path, T value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable && tunable.getTypeClass() == value.getClass()) {
        @SuppressWarnings("unchecked")
        Tunable<T> typedTunable = (Tunable<T>) tunable;
        m_actions.add(
            new Action(
                path,
                () -> {
                  typedTunable.set(value);
                  notifyOnTune(typedTunable);
                }));
      } else {
        throw new IllegalArgumentException(
            "Tunable at " + path + " is not a Tunable<" + value.getClass().getSimpleName() + ">");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setArray(String path, boolean[] value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable && tunable.getTypeClass() == boolean[].class) {
        @SuppressWarnings("unchecked")
        Tunable<boolean[]> typedTunable = (Tunable<boolean[]>) tunable;
        boolean[] copy = Arrays.copyOf(value, value.length);
        m_actions.add(
            new Action(
                path,
                () -> {
                  typedTunable.set(copy);
                  notifyOnTune(typedTunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a Tunable<boolean[]>");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setArray(String path, int[] value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable && tunable.getTypeClass() == int[].class) {
        @SuppressWarnings("unchecked")
        Tunable<int[]> typedTunable = (Tunable<int[]>) tunable;
        int[] copy = Arrays.copyOf(value, value.length);
        m_actions.add(
            new Action(
                path,
                () -> {
                  typedTunable.set(copy);
                  notifyOnTune(typedTunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a Tunable<int[]>");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setArray(String path, long[] value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable && tunable.getTypeClass() == long[].class) {
        @SuppressWarnings("unchecked")
        Tunable<long[]> typedTunable = (Tunable<long[]>) tunable;
        long[] copy = Arrays.copyOf(value, value.length);
        m_actions.add(
            new Action(
                path,
                () -> {
                  typedTunable.set(copy);
                  notifyOnTune(typedTunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a Tunable<long[]>");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setArray(String path, float[] value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable && tunable.getTypeClass() == float[].class) {
        @SuppressWarnings("unchecked")
        Tunable<float[]> typedTunable = (Tunable<float[]>) tunable;
        float[] copy = Arrays.copyOf(value, value.length);
        m_actions.add(
            new Action(
                path,
                () -> {
                  typedTunable.set(copy);
                  notifyOnTune(typedTunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a Tunable<float[]>");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setArray(String path, double[] value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable && tunable.getTypeClass() == double[].class) {
        @SuppressWarnings("unchecked")
        Tunable<double[]> typedTunable = (Tunable<double[]>) tunable;
        double[] copy = Arrays.copyOf(value, value.length);
        m_actions.add(
            new Action(
                path,
                () -> {
                  typedTunable.set(copy);
                  notifyOnTune(typedTunable);
                }));
      } else {
        throw new IllegalArgumentException("Tunable at " + path + " is not a Tunable<double[]>");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public <T> void setArray(String path, T[] value) {
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      if (rawTunable == null) {
        throw new IllegalArgumentException("No such tunable: " + path);
      }
      if (rawTunable instanceof Tunable<?> tunable && tunable.getTypeClass() == value.getClass()) {
        @SuppressWarnings("unchecked")
        Tunable<T[]> typedTunable = (Tunable<T[]>) tunable;
        T[] copy = Arrays.copyOf(value, value.length);
        m_actions.add(
            new Action(
                path,
                () -> {
                  typedTunable.set(copy);
                  notifyOnTune(typedTunable);
                }));
      } else {
        throw new IllegalArgumentException(
            "Tunable at " + path + " is not a Tunable<" + value.getClass().getSimpleName() + "[]>");
      }
    }
  }

  @Override
  public void publish(String path, TunableBase tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(path)) {
        throw new IllegalArgumentException("Tunable already exists: " + path);
      }
      m_entries.put(path, tunable);
    }
  }

  @Override
  public void publishComplex(String path, ComplexTunable tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(path)) {
        throw new IllegalArgumentException("Tunable already exists: " + path);
      }
      m_entries.put(path, tunable);
    }

    tunable.publishTunable(TunableRegistry.getTable(path));
  }

  @Override
  public void remove(String path) {
    synchronized (m_entries) {
      m_entries.remove(path);
    }
    synchronized (m_actions) {
      m_actions.removeIf(action -> action.path.equals(path));
    }
  }

  @Override
  public void update() {
    synchronized (m_actions) {
      for (Action action : m_actions) {
        action.update().run();
      }
      m_actions.clear();
    }
  }
}
