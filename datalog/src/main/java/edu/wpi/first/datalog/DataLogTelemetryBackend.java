// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.datalog;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.telemetry.TelemetryBackend;
import edu.wpi.first.util.telemetry.TelemetryEntry;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;
import us.hebi.quickbuf.ProtoMessage;

public class DataLogTelemetryBackend implements TelemetryBackend {
  private final DataLog m_log;
  private final String m_prefix;

  public DataLogTelemetryBackend(DataLog log, String prefix) {
    m_log = log;
    m_prefix = prefix;
  }

  @Override
  public TelemetryEntry getEntry(String name) {
    return new Entry(m_log, m_prefix + name);
  }

  private static class Entry implements TelemetryEntry {
    private final DataLog m_log;
    private final String m_path;
    private final AtomicReference<DataLogEntry> m_entry = new AtomicReference<>();
    private String m_typeString;
    private final AtomicBoolean m_keepDuplicates = new AtomicBoolean();
    private final Map<String, String> m_propertiesMap = new HashMap<>();
    private String m_properties = "{}";

    Entry(DataLog log, String path) {
      m_log = log;
      m_path = path;
    }

    @Override
    public void close() {
      var entry = m_entry.getAndSet(null);
      entry.finish();
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
            sb.append('"');
            sb.append(k.replace("\"", "\\\""));
            sb.append("\":");
            sb.append(v);
            sb.append(',');
          });
      // replace the trailing comma with a }
      sb.setCharAt(sb.length() - 1, '}');
      m_properties = sb.toString();
      DataLogEntry entry = m_entry.get();
      if (entry != null) {
        entry.setMetadata(m_properties);
      }
    }

    @Override
    public void setProperty(String key, String value) {
      synchronized (this) {
        String oldValue = m_propertiesMap.put(key, value);
        if (!value.equals(oldValue)) {
          refreshProperties();
        }
      }
    }

    @Override
    public void setTypeString(String typeString) {
      synchronized (this) {
        m_typeString = typeString;
      }
    }

    @Override
    public void logStruct(Object value, Struct<?> struct) {}

    @Override
    public <MessageType extends ProtoMessage<?>> void logProtobuf(
        Object value, Protobuf<?, MessageType> protobuf) {}

    @Override
    public void logStructArray(Object[] value, Struct<?> struct) {}

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
        // TODO: warn?
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
        // TODO: warn?
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
        // TODO: warn?
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
        // TODO: warn?
      }
    }

    @Override
    public void logString(String value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new StringLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof StringLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        // TODO: warn?
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
        // TODO: warn?
      }
    }

    @Override
    public void logByteArray(byte[] value) {
      DataLogEntry entry = m_entry.get();
      if (entry == null) {
        synchronized (this) {
          // double-check
          entry = m_entry.get();
          if (entry == null) {
            entry = new RawLogEntry(m_log, m_path, m_properties);
            m_entry.set(entry);
          }
        }
      }

      if (entry instanceof RawLogEntry e) {
        if (m_keepDuplicates.get()) {
          e.append(value);
        } else {
          e.update(value);
        }
      } else {
        // TODO: warn?
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
        // TODO: warn?
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
        // TODO: warn?
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
        // TODO: warn?
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
        // TODO: warn?
      }
    }
  }
}
