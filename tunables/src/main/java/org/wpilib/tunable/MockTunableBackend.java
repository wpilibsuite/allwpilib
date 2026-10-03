// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.IdentityHashMap;
import java.util.List;
import java.util.Map;
import org.wpilib.tunable.util.PathUtil;

/** A mock tunable backend for testing. */
public class MockTunableBackend implements TunableBackend {
  private final Map<String, Object> m_entries = new HashMap<>();

  private record Action(String path, TunableBase tunable, Runnable update) {}

  private final List<Action> m_actions = new ArrayList<>();

  /** Constructs a mock tunable backend. */
  public MockTunableBackend() {}

  private void addAction(Action action) {
    synchronized (m_actions) {
      m_actions.add(action);
    }
  }

  private List<Action> takeActions() {
    synchronized (m_actions) {
      List<Action> actions = new ArrayList<>(m_actions);
      m_actions.clear();
      return actions;
    }
  }

  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private boolean isCurrentAction(Action action) {
    synchronized (m_entries) {
      return m_entries.get(action.path) == action.tunable;
    }
  }

  /**
   * Gets a published tunable.
   *
   * @param path normalized path
   * @return tunable
   */
  public TunableBase getTunable(String path) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      return switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableBase tunable -> tunable;
        default -> throw new IllegalArgumentException("Tunable at " + path + " is not a tunable");
      };
    }
  }

  @Override
  public void close() {
    synchronized (m_entries) {
      m_entries.clear();
    }
  }

  private static void notifyOnTune(TunableBase tunable) {
    TunableRegistry.resetChangedAfterUpdate(tunable);
    var config = tunable.getConfig();
    if (config != null) {
      var onTune = config.getOnTune();
      if (onTune != null) {
        TunableRegistry.runAfterUpdate(onTune);
      }
    }
  }

  private static boolean isMutable(TunableBase tunable) {
    var config = tunable.getConfig();
    return config == null || config.isMutable();
  }

  private static boolean hasTypeClass(Tunable<?> tunable, Class<?> cls) {
    Class<?> tunableTypeClass = tunable.getTypeClass();
    return tunableTypeClass != null && cls.isAssignableFrom(tunableTypeClass);
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public boolean getBoolean(String path) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      return switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableBoolean tunable -> tunable.get();
        default ->
            throw new IllegalArgumentException("Tunable at " + path + " is not a TunableBoolean");
      };
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public int getInteger(String path) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      return switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableInt tunable -> tunable.get();
        default ->
            throw new IllegalArgumentException("Tunable at " + path + " is not a TunableInt");
      };
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public long getLong(String path) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      return switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableLong tunable -> tunable.get();
        default ->
            throw new IllegalArgumentException("Tunable at " + path + " is not a TunableLong");
      };
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public float getFloat(String path) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      return switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableFloat tunable -> tunable.get();
        default ->
            throw new IllegalArgumentException("Tunable at " + path + " is not a TunableFloat");
      };
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param path normalized path
   * @return value
   */
  public double getDouble(String path) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      return switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableDouble tunable -> tunable.get();
        default ->
            throw new IllegalArgumentException("Tunable at " + path + " is not a TunableDouble");
      };
    }
  }

  /**
   * Gets the value of a tunable.
   *
   * @param <T> value type
   * @param path normalized path
   * @param cls expected value class, superclass, or interface
   * @return value
   */
  public <T> T getValue(String path, Class<T> cls) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      return switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when hasTypeClass(tunable, cls) -> cls.cast(tunable.get());
        default ->
            throw new IllegalArgumentException(
                "Tunable at " + path + " is not a Tunable<" + cls.getSimpleName() + ">");
      };
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param path normalized path
   * @param value value
   */
  public void setBoolean(String path, boolean value) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableBoolean tunable ->
            addAction(
                new Action(
                    path,
                    tunable,
                    () -> {
                      if (isMutable(tunable)) {
                        tunable.set(value);
                        notifyOnTune(tunable);
                      }
                    }));
        default ->
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableInt tunable ->
            addAction(
                new Action(
                    path,
                    tunable,
                    () -> {
                      if (isMutable(tunable)) {
                        tunable.set(value);
                        notifyOnTune(tunable);
                      }
                    }));
        default ->
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableLong tunable ->
            addAction(
                new Action(
                    path,
                    tunable,
                    () -> {
                      if (isMutable(tunable)) {
                        tunable.set(value);
                        notifyOnTune(tunable);
                      }
                    }));
        default ->
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableFloat tunable ->
            addAction(
                new Action(
                    path,
                    tunable,
                    () -> {
                      if (isMutable(tunable)) {
                        tunable.set(value);
                        notifyOnTune(tunable);
                      }
                    }));
        default ->
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case TunableDouble tunable ->
            addAction(
                new Action(
                    path,
                    tunable,
                    () -> {
                      if (isMutable(tunable)) {
                        tunable.set(value);
                        notifyOnTune(tunable);
                      }
                    }));
        default ->
            throw new IllegalArgumentException("Tunable at " + path + " is not a TunableDouble");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param <T> value type
   * @param path normalized path
   * @param value value
   */
  public <T> void setValue(String path, T value) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when tunable.getTypeClass() == value.getClass() -> {
          @SuppressWarnings("unchecked")
          Tunable<T> typedTunable = (Tunable<T>) tunable;
          addAction(
              new Action(
                  path,
                  typedTunable,
                  () -> {
                    if (isMutable(typedTunable)) {
                      typedTunable.set(value);
                      notifyOnTune(typedTunable);
                    }
                  }));
        }
        default ->
            throw new IllegalArgumentException(
                "Tunable at "
                    + path
                    + " is not a Tunable<"
                    + value.getClass().getSimpleName()
                    + ">");
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when tunable.getTypeClass() == boolean[].class -> {
          @SuppressWarnings("unchecked")
          Tunable<boolean[]> typedTunable = (Tunable<boolean[]>) tunable;
          boolean[] copy = Arrays.copyOf(value, value.length);
          addAction(
              new Action(
                  path,
                  typedTunable,
                  () -> {
                    if (isMutable(typedTunable)) {
                      typedTunable.set(copy);
                      notifyOnTune(typedTunable);
                    }
                  }));
        }
        default ->
            throw new IllegalArgumentException(
                "Tunable at " + path + " is not a Tunable<boolean[]>");
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when tunable.getTypeClass() == int[].class -> {
          @SuppressWarnings("unchecked")
          Tunable<int[]> typedTunable = (Tunable<int[]>) tunable;
          int[] copy = Arrays.copyOf(value, value.length);
          addAction(
              new Action(
                  path,
                  typedTunable,
                  () -> {
                    if (isMutable(typedTunable)) {
                      typedTunable.set(copy);
                      notifyOnTune(typedTunable);
                    }
                  }));
        }
        default ->
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when tunable.getTypeClass() == long[].class -> {
          @SuppressWarnings("unchecked")
          Tunable<long[]> typedTunable = (Tunable<long[]>) tunable;
          long[] copy = Arrays.copyOf(value, value.length);
          addAction(
              new Action(
                  path,
                  typedTunable,
                  () -> {
                    if (isMutable(typedTunable)) {
                      typedTunable.set(copy);
                      notifyOnTune(typedTunable);
                    }
                  }));
        }
        default ->
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when tunable.getTypeClass() == float[].class -> {
          @SuppressWarnings("unchecked")
          Tunable<float[]> typedTunable = (Tunable<float[]>) tunable;
          float[] copy = Arrays.copyOf(value, value.length);
          addAction(
              new Action(
                  path,
                  typedTunable,
                  () -> {
                    if (isMutable(typedTunable)) {
                      typedTunable.set(copy);
                      notifyOnTune(typedTunable);
                    }
                  }));
        }
        default ->
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
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when tunable.getTypeClass() == double[].class -> {
          @SuppressWarnings("unchecked")
          Tunable<double[]> typedTunable = (Tunable<double[]>) tunable;
          double[] copy = Arrays.copyOf(value, value.length);
          addAction(
              new Action(
                  path,
                  typedTunable,
                  () -> {
                    if (isMutable(typedTunable)) {
                      typedTunable.set(copy);
                      notifyOnTune(typedTunable);
                    }
                  }));
        }
        default ->
            throw new IllegalArgumentException(
                "Tunable at " + path + " is not a Tunable<double[]>");
      }
    }
  }

  /**
   * Sets the value of a tunable.
   *
   * @param <T> array element type
   * @param path normalized path
   * @param value value
   */
  public <T> void setArray(String path, T[] value) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      var rawTunable = m_entries.get(path);
      switch (rawTunable) {
        case null -> throw new IllegalArgumentException("No such tunable: " + path);
        case Tunable<?> tunable when tunable.getTypeClass() == value.getClass() -> {
          @SuppressWarnings("unchecked")
          Tunable<T[]> typedTunable = (Tunable<T[]>) tunable;
          T[] copy = Arrays.copyOf(value, value.length);
          addAction(
              new Action(
                  path,
                  typedTunable,
                  () -> {
                    if (isMutable(typedTunable)) {
                      typedTunable.set(copy);
                      notifyOnTune(typedTunable);
                    }
                  }));
        }
        default ->
            throw new IllegalArgumentException(
                "Tunable at "
                    + path
                    + " is not a Tunable<"
                    + value.getClass().getSimpleName()
                    + "[]>");
      }
    }
  }

  @Override
  public boolean publish(String path, TunableBase tunable) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      if (m_entries.containsKey(path)) {
        TunableRegistry.reportWarning("Tunable already exists: " + path);
        return false;
      }
      m_entries.put(path, tunable);
    }
    return true;
  }

  @Override
  public boolean publishComplex(String path, ComplexTunable tunable) {
    path = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      if (m_entries.containsKey(path)) {
        TunableRegistry.reportWarning("Tunable already exists: " + path);
        return false;
      }
      m_entries.put(path, tunable);
    }

    TunableRegistry.publishComplexChildren(path, tunable);
    return true;
  }

  @Override
  public void remove(String path) {
    String normalizedPath = PathUtil.normalizeName(path);
    synchronized (m_entries) {
      m_entries.remove(normalizedPath);
    }
    synchronized (m_actions) {
      m_actions.removeIf(action -> action.path.equals(normalizedPath));
    }
  }

  @Override
  public List<PublishedTunable> removePrefix(String prefix) {
    String normalizedPrefix = PathUtil.normalizePrefix(prefix);
    List<PublishedTunable> removed = new ArrayList<>();
    synchronized (m_entries) {
      var iterator = m_entries.entrySet().iterator();
      while (iterator.hasNext()) {
        var entry = iterator.next();
        if (!PathUtil.isPathOrDescendant(entry.getKey(), normalizedPrefix)) {
          continue;
        }
        switch (entry.getValue()) {
          case ComplexTunable tunable ->
              removed.add(new PublishedTunable(entry.getKey(), null, tunable));
          case TunableBase tunable ->
              removed.add(new PublishedTunable(entry.getKey(), tunable, null));
          default -> TunableRegistry.reportWarning("Unexpected tunable entry type");
        }
        iterator.remove();
      }
    }
    synchronized (m_actions) {
      m_actions.removeIf(action -> PathUtil.isPathOrDescendant(action.path, normalizedPrefix));
    }
    return removed;
  }

  @Override
  public void update() {
    List<Action> actions = takeActions();
    for (Action action : actions) {
      if (isCurrentAction(action)) {
        action.update().run();
      }
    }

    List<ComplexTunable> complexTunables = new ArrayList<>();
    synchronized (m_entries) {
      IdentityHashMap<ComplexTunable, Boolean> seenComplexTunables = new IdentityHashMap<>();
      for (Object entry : m_entries.values()) {
        if (entry instanceof ComplexTunable tunable
            && seenComplexTunables.put(tunable, Boolean.TRUE) == null) {
          complexTunables.add(tunable);
        }
      }
    }
    for (ComplexTunable tunable : complexTunables) {
      TunableRegistry.updateComplexIfNeeded(tunable);
    }
  }
}
