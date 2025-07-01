// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import java.util.HashMap;
import java.util.Map;
import org.wpilib.datalog.BooleanArrayLogEntry;
import org.wpilib.datalog.BooleanLogEntry;
import org.wpilib.datalog.DataLog;
import org.wpilib.datalog.DataLogEntry;
import org.wpilib.datalog.DoubleArrayLogEntry;
import org.wpilib.datalog.DoubleLogEntry;
import org.wpilib.datalog.FloatArrayLogEntry;
import org.wpilib.datalog.FloatLogEntry;
import org.wpilib.datalog.IntegerArrayLogEntry;
import org.wpilib.datalog.IntegerLogEntry;
import org.wpilib.datalog.ProtobufLogEntry;
import org.wpilib.datalog.RawLogEntry;
import org.wpilib.datalog.StringArrayLogEntry;
import org.wpilib.datalog.StringLogEntry;
import org.wpilib.datalog.StructArrayLogEntry;
import org.wpilib.datalog.StructLogEntry;
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

  @SuppressWarnings({"PMD.SingularField", "PMD.TooFewBranchesForSwitch"})
  private static final class Entry implements TelemetryEntry {
    private final DataLog m_log;
    private final String m_path;
    private volatile DataLogEntry m_entry;
    private volatile String m_typeString;
    private volatile boolean m_keepDuplicates;
    private final Map<String, String> m_propertiesMap = new HashMap<>();
    private String m_properties = "{}";
    private Struct<?> m_struct;
    private Protobuf<?, ?> m_proto;

    Entry(DataLog log, String path) {
      m_log = log;
      m_path = path;
    }

    public void close() {
      DataLogEntry entry;
      synchronized (this) {
        entry = m_entry;
        m_entry = null;
      }
      if (entry != null) {
        entry.finish();
      }
    }

    @Override
    public void keepDuplicates() {
      m_keepDuplicates = true;
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
          DataLogEntry entry = m_entry;
          if (entry != null) {
            entry.setMetadata(m_properties);
          }
        }
      }
    }

    private synchronized <T> StructLogEntry<T> initStruct(Struct<T> struct) {
      DataLogEntry entry = m_entry;
      return switch (entry) {
        case null -> {
          StructLogEntry<T> e = StructLogEntry.create(m_log, m_path, struct, m_properties);
          m_struct = struct;
          m_entry = e;
          yield e;
        }
        case StructLogEntry<?> e when struct.equals(m_struct) -> {
          @SuppressWarnings("unchecked")
          StructLogEntry<T> typedEntry = (StructLogEntry<T>) e;
          yield typedEntry;
        }
        default -> null;
      };
    }

    @Override
    public <T> void logStruct(T value, Struct<T> struct) {
      StructLogEntry<T> entry = initStruct(struct);
      if (entry != null) {
        if (m_keepDuplicates) {
          entry.append(value);
        } else {
          entry.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private synchronized <T> ProtobufLogEntry<T> initProtobuf(Protobuf<T, ?> proto) {
      DataLogEntry entry = m_entry;
      return switch (entry) {
        case null -> {
          ProtobufLogEntry<T> e = ProtobufLogEntry.create(m_log, m_path, proto, m_properties);
          m_proto = proto;
          m_entry = e;
          yield e;
        }
        case ProtobufLogEntry<?> e when proto.equals(m_proto) -> {
          @SuppressWarnings("unchecked")
          ProtobufLogEntry<T> typedEntry = (ProtobufLogEntry<T>) e;
          yield typedEntry;
        }
        default -> null;
      };
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {
      ProtobufLogEntry<T> entry = initProtobuf(proto);
      if (entry != null) {
        if (m_keepDuplicates) {
          entry.append(value);
        } else {
          entry.update(value);
        }
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private synchronized <T> StructArrayLogEntry<T> initStructArray(Struct<T> struct) {
      DataLogEntry entry = m_entry;
      return switch (entry) {
        case null -> {
          StructArrayLogEntry<T> e =
              StructArrayLogEntry.create(m_log, m_path, struct, m_properties);
          m_entry = e;
          yield e;
        }
        case StructArrayLogEntry<?> e when struct.equals(m_struct) -> {
          @SuppressWarnings("unchecked")
          StructArrayLogEntry<T> typedEntry = (StructArrayLogEntry<T>) e;
          yield typedEntry;
        }
        default -> null;
      };
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<T> struct) {
      StructArrayLogEntry<T> entry = initStructArray(struct);
      if (entry != null) {
        if (m_keepDuplicates) {
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
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new BooleanLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case BooleanLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logLong(long value) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new IntegerLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case IntegerLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloat(float value) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new FloatLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case FloatLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDouble(double value) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new DoubleLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case DoubleLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logString(String value, String typeString) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            m_typeString = typeString;
            entry = new StringLogEntry(m_log, m_path, m_properties, m_typeString);
            m_entry = entry;
          }
        }
      }

      String curTypeString = m_typeString;

      switch (entry) {
        case StringLogEntry e when curTypeString.equals(typeString) -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logBooleanArray(boolean[] value) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new BooleanArrayLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case BooleanArrayLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
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
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new IntegerArrayLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case IntegerArrayLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloatArray(float[] value) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new FloatArrayLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case FloatArrayLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDoubleArray(double[] value) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new DoubleArrayLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case DoubleArrayLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logStringArray(String[] value) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            entry = new StringArrayLogEntry(m_log, m_path, m_properties);
            m_entry = entry;
          }
        }
      }

      switch (entry) {
        case StringArrayLogEntry e -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logRaw(byte[] value, String typeString) {
      DataLogEntry entry = m_entry;
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry;
          if (entry == null) {
            m_typeString = typeString;
            entry = new RawLogEntry(m_log, m_path, m_properties, m_typeString);
            m_entry = entry;
          }
        }
      }

      String curTypeString = m_typeString;

      switch (entry) {
        case RawLogEntry e when curTypeString.equals(typeString) -> {
          if (m_keepDuplicates) {
            e.append(value);
          } else {
            e.update(value);
          }
        }
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }
  }
}
