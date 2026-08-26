// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.IdentityHashMap;
import java.util.List;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/** A telemetry backend that forwards logged data to multiple child backends. */
public class MultiTelemetryBackend implements TelemetryBackend {
  private final List<TelemetryBackend> m_backends;
  private final ConcurrentMap<String, Entry> m_entries = new ConcurrentHashMap<>();

  /**
   * Constructs a telemetry backend that forwards to multiple child backends.
   *
   * @param backends child backends
   */
  public MultiTelemetryBackend(TelemetryBackend... backends) {
    this(Arrays.asList(Objects.requireNonNull(backends, "backends")));
  }

  /**
   * Constructs a telemetry backend that forwards to multiple child backends.
   *
   * @param backends child backends
   */
  public MultiTelemetryBackend(Collection<? extends TelemetryBackend> backends) {
    m_backends = List.copyOf(Objects.requireNonNull(backends, "backends"));
  }

  @Override
  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  public boolean ownsBackend(TelemetryBackend backend) {
    if (this == backend) {
      return true;
    }
    for (TelemetryBackend child : m_backends) {
      if (child.ownsBackend(backend)) {
        return true;
      }
    }
    return false;
  }

  @Override
  public boolean sharesBackendWith(TelemetryBackend backend) {
    if (TelemetryBackend.super.sharesBackendWith(backend)) {
      return true;
    }
    for (TelemetryBackend child : m_backends) {
      if (child.sharesBackendWith(backend)) {
        return true;
      }
    }
    return false;
  }

  @Override
  public void close() throws Exception {
    close(Collections.newSetFromMap(new IdentityHashMap<>()), Collections.emptySet());
  }

  @SuppressWarnings({"PMD.AvoidCatchingGenericException", "PMD.SignatureDeclareThrowsException"})
  void close(Set<TelemetryBackend> closedBackends, Set<TelemetryBackend> skipBackends)
      throws Exception {
    if (skipBackends.contains(this) || !closedBackends.add(this)) {
      return;
    }

    for (Entry entry : m_entries.values()) {
      entry.close();
    }
    m_entries.clear();

    Exception exception = null;
    for (TelemetryBackend backend : m_backends) {
      try {
        TelemetryRegistry.closeBackend(backend, closedBackends, skipBackends);
      } catch (Exception e) {
        if (exception == null) {
          exception = e;
        } else {
          exception.addSuppressed(e);
        }
      }
    }
    if (exception != null) {
      throw exception;
    }
  }

  void collectOwnedBackends(Set<TelemetryBackend> ownedBackends) {
    if (!ownedBackends.add(this)) {
      return;
    }
    for (TelemetryBackend backend : m_backends) {
      TelemetryRegistry.collectOwnedBackends(backend, ownedBackends);
    }
  }

  @Override
  public TelemetryEntry getEntry(String path) {
    return m_entries.computeIfAbsent(path, k -> new Entry(m_backends, k));
  }

  @Override
  public void removeEntry(String path) {
    Entry entry = m_entries.remove(path);
    if (entry != null) {
      entry.close();
    }
    for (TelemetryBackend backend : m_backends) {
      backend.removeEntry(path);
    }
  }

  @SuppressWarnings("PMD.ForLoopCanBeForeach")
  private static final class Entry implements TelemetryEntry {
    private final List<TelemetryEntry> m_entries;
    private volatile boolean m_closed;

    Entry(List<TelemetryBackend> backends, String path) {
      List<TelemetryEntry> entries = new ArrayList<>(backends.size());
      for (TelemetryBackend backend : backends) {
        entries.add(Objects.requireNonNull(backend.getEntry(path), "backend entry"));
      }
      m_entries = List.copyOf(entries);
    }

    void close() {
      m_closed = true;
    }

    @Override
    public boolean isDiscard() {
      if (m_closed) {
        return true;
      }
      for (TelemetryEntry entry : m_entries) {
        if (!entry.isDiscard()) {
          return false;
        }
      }
      return true;
    }

    @Override
    public void keepDuplicates() {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).keepDuplicates();
      }
    }

    @Override
    public void setProperty(String key, String value) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).setProperty(key, value);
      }
    }

    @Override
    public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logStruct(value, struct, timestamp);
      }
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logProtobuf(value, proto, timestamp);
      }
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logStructArray(value, struct, timestamp);
      }
    }

    @Override
    public void logBoolean(boolean value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logBoolean(value, timestamp);
      }
    }

    @Override
    public void logByte(byte value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logByte(value, timestamp);
      }
    }

    @Override
    public void logShort(short value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logShort(value, timestamp);
      }
    }

    @Override
    public void logInt(int value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logInt(value, timestamp);
      }
    }

    @Override
    public void logLong(long value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logLong(value, timestamp);
      }
    }

    @Override
    public void logFloat(float value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logFloat(value, timestamp);
      }
    }

    @Override
    public void logDouble(double value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logDouble(value, timestamp);
      }
    }

    @Override
    public void logString(String value, String typeString, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logString(value, typeString, timestamp);
      }
    }

    @Override
    public void logBooleanArray(boolean[] value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logBooleanArray(value, timestamp);
      }
    }

    @Override
    public void logShortArray(short[] value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logShortArray(value, timestamp);
      }
    }

    @Override
    public void logIntArray(int[] value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logIntArray(value, timestamp);
      }
    }

    @Override
    public void logLongArray(long[] value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logLongArray(value, timestamp);
      }
    }

    @Override
    public void logFloatArray(float[] value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logFloatArray(value, timestamp);
      }
    }

    @Override
    public void logDoubleArray(double[] value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logDoubleArray(value, timestamp);
      }
    }

    @Override
    public void logStringArray(String[] value, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logStringArray(value, timestamp);
      }
    }

    @Override
    public void logRaw(byte[] value, String typeString, long timestamp) {
      if (m_closed) {
        return;
      }
      for (int i = 0; i < m_entries.size(); i++) {
        if (m_closed) {
          return;
        }
        m_entries.get(i).logRaw(value, typeString, timestamp);
      }
    }
  }
}
