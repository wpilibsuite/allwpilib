// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import io.avaje.json.JsonException;
import io.avaje.jsonb.JsonType;
import io.avaje.jsonb.Jsonb;
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
  private static final Jsonb s_json = Jsonb.instance();
  private static final JsonType<Object> s_jsonValue = s_json.type(Object.class);

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

  private static boolean isValidPropertyJson(String value) {
    try {
      s_jsonValue.fromJson(value);
      return true;
    } catch (JsonException e) {
      return false;
    }
  }

  @Override
  public void removeEntry(String path) {
    Entry entry;
    synchronized (m_entries) {
      entry = m_entries.remove(path);
    }
    if (entry != null) {
      entry.close();
    }
  }

  @SuppressWarnings({
    "PMD.AvoidArrayLoops",
    "PMD.AvoidCatchingGenericException",
    "PMD.SingularField",
    "PMD.TooFewBranchesForSwitch"
  })
  private static final class Entry implements TelemetryEntry {
    private final DataLog m_log;
    private final String m_path;
    private volatile DataLogEntry m_entry;
    private volatile boolean m_closed;
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
      synchronized (this) {
        DataLogEntry entry = m_entry;
        m_entry = null;
        m_closed = true;
        if (entry != null) {
          entry.finish();
        }
      }
    }

    @Override
    public boolean isDiscard() {
      return m_closed;
    }

    @Override
    public synchronized void keepDuplicates() {
      if (!m_closed) {
        m_keepDuplicates = true;
      }
    }

    synchronized void refreshProperties() {
      StringBuilder sb = new StringBuilder();
      sb.append('{');
      m_propertiesMap.forEach(
          (k, v) -> {
            sb.append(s_json.toJson(k)).append(':').append(v).append(',');
          });
      // replace the trailing comma with a }
      sb.setCharAt(sb.length() - 1, '}');
      m_properties = sb.toString();
    }

    @Override
    public void setProperty(String key, String value) {
      if (m_closed) {
        return;
      }
      if (!isValidPropertyJson(value)) {
        TelemetryRegistry.reportWarning(m_path, "invalid property JSON");
        return;
      }
      synchronized (this) {
        if (m_closed) {
          return;
        }
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

    private <T> StructLogEntry<T> initStruct(Struct<T> struct, long timestamp) {
      if (m_closed) {
        return null;
      }
      DataLogEntry entry = m_entry;
      return switch (entry) {
        case null -> {
          StructLogEntry<T> e =
              StructLogEntry.create(m_log, m_path, struct, m_properties, timestamp);
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
    public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {
      boolean typeMismatch = false;
      try {
        synchronized (this) {
          StructLogEntry<? super T> entry = initStruct(struct, timestamp);
          if (entry != null) {
            if (m_keepDuplicates) {
              entry.append(value, timestamp);
            } else {
              entry.update(value, timestamp);
            }
          } else if (!m_closed) {
            typeMismatch = true;
          }
        }
        if (typeMismatch) {
          reportTypeMismatch();
        }
      } catch (RuntimeException e) {
        reportWarning("failed to publish struct value", e);
      }
    }

    private <T> ProtobufLogEntry<T> initProtobuf(Protobuf<T, ?> proto, long timestamp) {
      if (m_closed) {
        return null;
      }
      DataLogEntry entry = m_entry;
      return switch (entry) {
        case null -> {
          ProtobufLogEntry<T> e =
              ProtobufLogEntry.create(m_log, m_path, proto, m_properties, timestamp);
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
    public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {
      boolean typeMismatch = false;
      try {
        synchronized (this) {
          ProtobufLogEntry<? super T> entry = initProtobuf(proto, timestamp);
          if (entry != null) {
            if (m_keepDuplicates) {
              entry.append(value, timestamp);
            } else {
              entry.update(value, timestamp);
            }
          } else if (!m_closed) {
            typeMismatch = true;
          }
        }
        if (typeMismatch) {
          reportTypeMismatch();
        }
      } catch (RuntimeException e) {
        reportWarning("failed to publish protobuf value", e);
      }
    }

    private <T> StructArrayLogEntry<T> initStructArray(Struct<T> struct, long timestamp) {
      if (m_closed) {
        return null;
      }
      DataLogEntry entry = m_entry;
      return switch (entry) {
        case null -> {
          StructArrayLogEntry<T> e =
              StructArrayLogEntry.create(m_log, m_path, struct, m_properties, timestamp);
          m_struct = struct;
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
    public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {
      boolean typeMismatch = false;
      try {
        synchronized (this) {
          StructArrayLogEntry<? super T> entry = initStructArray(struct, timestamp);
          if (entry != null) {
            if (m_keepDuplicates) {
              entry.append(value, timestamp);
            } else {
              entry.update(value, timestamp);
            }
          } else if (!m_closed) {
            typeMismatch = true;
          }
        }
        if (typeMismatch) {
          reportTypeMismatch();
        }
      } catch (RuntimeException e) {
        reportWarning("failed to publish struct array value", e);
      }
    }

    private void reportWarning(String msg, RuntimeException exception) {
      String exceptionMessage = exception.getMessage();
      TelemetryRegistry.reportWarning(
          m_path, exceptionMessage == null ? msg : msg + ": " + exceptionMessage);
    }

    private void reportTypeMismatch() {
      TelemetryRegistry.reportWarning(m_path, "type mismatch");
    }

    @Override
    public void logBoolean(boolean value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new BooleanLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case BooleanLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logLong(long value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new IntegerLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case IntegerLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logFloat(float value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new FloatLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case FloatLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logDouble(double value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new DoubleLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case DoubleLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logString(String value, String typeString, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          m_typeString = typeString;
          entry = new StringLogEntry(m_log, m_path, m_properties, m_typeString, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case StringLogEntry e when m_typeString.equals(typeString) -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logBooleanArray(boolean[] value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new BooleanArrayLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case BooleanArrayLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logShortArray(short[] value, long timestamp) {
      logLongArray(toLongArray(value), timestamp);
    }

    @Override
    public void logIntArray(int[] value, long timestamp) {
      logLongArray(toLongArray(value), timestamp);
    }

    @Override
    public void logLongArray(long[] value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new IntegerArrayLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case IntegerArrayLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    private static long[] toLongArray(short[] arr) {
      long[] result = new long[arr.length];
      for (int i = 0; i < arr.length; i++) {
        result[i] = arr[i];
      }
      return result;
    }

    private static long[] toLongArray(int[] arr) {
      long[] result = new long[arr.length];
      for (int i = 0; i < arr.length; i++) {
        result[i] = arr[i];
      }
      return result;
    }

    @Override
    public void logFloatArray(float[] value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new FloatArrayLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case FloatArrayLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logDoubleArray(double[] value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new DoubleArrayLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case DoubleArrayLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logStringArray(String[] value, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          entry = new StringArrayLogEntry(m_log, m_path, m_properties, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case StringArrayLogEntry e -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }

    @Override
    public void logRaw(byte[] value, String typeString, long timestamp) {
      boolean typeMismatch = false;
      synchronized (this) {
        if (m_closed) {
          return;
        }

        DataLogEntry entry = m_entry;
        if (entry == null) {
          m_typeString = typeString;
          entry = new RawLogEntry(m_log, m_path, m_properties, m_typeString, timestamp);
          m_entry = entry;
        }

        switch (entry) {
          case RawLogEntry e when m_typeString.equals(typeString) -> {
            if (m_keepDuplicates) {
              e.append(value, timestamp);
            } else {
              e.update(value, timestamp);
            }
          }
          default -> typeMismatch = true;
        }
      }
      if (typeMismatch) {
        reportTypeMismatch();
      }
    }
  }
}
