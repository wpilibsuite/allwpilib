// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;
import org.wpilib.telemetry.TelemetryBackend;
import org.wpilib.telemetry.TelemetryEntry;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/** A telemetry backend that sends logged data to a DataLog. */
public class DataLogTelemetryBackend implements TelemetryBackend {
  private final DataLog m_log;
  private final String m_prefix;
  private final Map<String, Entry> m_entries = new HashMap<>();

  /**
   * Construct.
   *
   * @param log datalog
   * @param prefix prefix to put in front of logged path in data log
   */
  public DataLogTelemetryBackend(DataLog log, String prefix) {
    m_log = log;
    m_prefix = prefix;
  }

  @Override
  public void close() {
    synchronized (m_entries) {
      for (Entry entry : m_entries.values()) {
        entry.close();
      }
      m_entries.clear();
    }
  }

  @Override
  public TelemetryEntry getEntry(String path) {
    synchronized (m_entries) {
      return m_entries.computeIfAbsent(path, k -> new Entry(m_log, m_prefix + k));
    }
  }

  private static final class Entry implements TelemetryEntry {
    private final DataLog m_log;
    private final String m_path;
    private final AtomicReference<DataLogEntry> m_entry = new AtomicReference<>();
    private String m_typeString;
    private final AtomicBoolean m_keepDuplicates = new AtomicBoolean();
    private final Map<String, String> m_propertiesMap = new HashMap<>();
    private String m_properties = "{}";
    private Struct<?> m_struct;
    private Protobuf<?, ?> m_proto;

    Entry(DataLog log, String path) {
      m_log = log;
      m_path = path;
    }

    public void close() {
      var entry = m_entry.getAndSet(null);
      if (entry != null) {
        entry.finish();
      }
    }

    @Override
    public void keepDuplicates() {
      m_keepDuplicates.set(true);
    }

    synchronized void refreshProperties() {
      StringBuilder sb = new StringBuilder();
      sb.append('{');
      m_propertiesMap.forEach(
          (k, v) -> {
            sb.append('"').append(k.replace("\"", "\\\"")).append("\":").append(v).append(',');
          });
      // replace the trailing comma with a }
      sb.setCharAt(sb.length() - 1, '}');
      m_properties = sb.toString();
    }

    @Override
    public void setProperty(String key, String value) {
      synchronized (this) {
        String oldValue = m_propertiesMap.put(key, value);
        if (!value.equals(oldValue)) {
          refreshProperties();
          DataLogEntry entry = m_entry.get();
          if (entry != null) {
            entry.setMetadata(m_properties);
          }
        }
      }
    }

    private synchronized <T> StructLogEntry<T> initStruct(Struct<T> struct) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        StructLogEntry<T> e = StructLogEntry.create(m_log, m_path, struct, m_properties);
        m_struct = struct;
        m_entry.set(e);
        return e;
      } else if (entry instanceof StructLogEntry<?> && struct.equals(m_struct)) {
        @SuppressWarnings("unchecked")
        StructLogEntry<T> e = (StructLogEntry<T>) entry;
        return e;
      } else {
        return null;
      }
    }

    @Override
    public <T> void logStruct(T value, Struct<T> struct) {
      StructLogEntry<T> entry = initStruct(struct);
      if (entry != null) {
        if (m_keepDuplicates.get()) {
          entry.append(value);
        } else {
          entry.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private synchronized <T> ProtobufLogEntry<T> initProtobuf(Protobuf<T, ?> proto) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        ProtobufLogEntry<T> e = ProtobufLogEntry.create(m_log, m_path, proto, m_properties);
        m_proto = proto;
        m_entry.set(e);
        return e;
      } else if (entry instanceof ProtobufLogEntry<?> && proto.equals(m_proto)) {
        @SuppressWarnings("unchecked")
        ProtobufLogEntry<T> e = (ProtobufLogEntry<T>) entry;
        return e;
      } else {
        return null;
      }
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {
      ProtobufLogEntry<T> entry = initProtobuf(proto);
      if (entry != null) {
        if (m_keepDuplicates.get()) {
          entry.append(value);
        } else {
          entry.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private synchronized <T> StructArrayLogEntry<T> initStructArray(Struct<T> struct) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        StructArrayLogEntry<T> e = StructArrayLogEntry.create(m_log, m_path, struct, m_properties);
        m_entry.set(e);
        return e;
      } else if (entry instanceof StructArrayLogEntry<?> && struct.equals(m_struct)) {
        @SuppressWarnings("unchecked")
        StructArrayLogEntry<T> e = (StructArrayLogEntry<T>) entry;
        return e;
      } else {
        return null;
      }
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<T> struct) {
      StructArrayLogEntry<T> entry = initStructArray(struct);
      if (entry != null) {
        if (m_keepDuplicates.get()) {
          entry.append(value);
        } else {
          entry.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logBoolean(boolean value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new BooleanLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof BooleanLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logLong(long value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new IntegerLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof IntegerLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloat(float value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new FloatLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof FloatLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDouble(double value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new DoubleLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof DoubleLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logString(String value, String typeString) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            m_typeString = typeString;
            entry = new StringLogEntry(m_log, m_path, m_properties, m_typeString);
            m_entry.set(entry);
          }
        }
      }

      String curTypeString;
      synchronized (this) {
        curTypeString = m_typeString;
      }

      if (entry instanceof StringLogEntry e && curTypeString.equals(typeString)) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logBooleanArray(boolean[] value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new BooleanArrayLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof BooleanArrayLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logShortArray(short[] value) {
      // TODO
    }

    @Override
    public void logIntArray(int[] value) {
      // TODO
    }

    @Override
    public void logLongArray(long[] value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new IntegerArrayLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof IntegerArrayLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloatArray(float[] value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new FloatArrayLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof FloatArrayLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDoubleArray(double[] value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new DoubleArrayLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof DoubleArrayLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logStringArray(String[] value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new StringArrayLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof StringArrayLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logRaw(byte[] value, String typeString) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            m_typeString = typeString;
            entry = new RawLogEntry(m_log, m_path, m_properties, m_typeString);
            m_entry.set(entry);
          }
        }
      }

      String curTypeString;
      synchronized (this) {
        curTypeString = m_typeString;
      }

      if (entry instanceof RawLogEntry e && curTypeString.equals(typeString)) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }
  }
}
