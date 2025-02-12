// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.telemetry.TelemetryBackend;
import edu.wpi.first.util.telemetry.TelemetryEntry;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;

public class NetworkTablesTelemetryBackend implements TelemetryBackend {
  private final NetworkTableInstance m_inst;
  private final String m_prefix;

  public NetworkTablesTelemetryBackend(NetworkTableInstance inst, String prefix) {
    m_inst = inst;
    m_prefix = prefix;
  }

  @Override
  public TelemetryEntry getEntry(String name) {
    return new Entry(m_inst, m_prefix + name);
  }

  private static class Entry implements TelemetryEntry {
    private final NetworkTableInstance m_inst;
    private final String m_path;
    private final AtomicReference<Publisher> m_pub = new AtomicReference<>();
    private String m_typeString;
    private final AtomicBoolean m_keepDuplicates = new AtomicBoolean();
    private final Map<String, String> m_propertiesMap = new HashMap<>();
    private String m_properties = "{}";
    private Struct<?> m_struct;
    private Protobuf<?, ?> m_proto;

    Entry(NetworkTableInstance inst, String path) {
      m_inst = inst;
      m_path = path;
    }

    @Override
    public void close() {
      var pub = m_pub.getAndSet(null);
      if (pub != null) {
        pub.close();
      }
    }

    @Override
    public void keepDuplicates() {
      m_keepDuplicates.set(true);
      // TODO: update publisher
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
      Publisher pub = m_pub.get();
      if (pub != null) {
        pub.getTopic().setProperties(m_properties);
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
    public <T> void logStruct(T value, Struct<T> struct) {}

    @Override
    public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {}

    @Override
    public <T> void logStructArray(T[] value, Struct<T> struct) {}

    @Override
    public void logBoolean(boolean value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getBooleanTopic(m_path)
                    .publishEx(
                        m_typeString != null ? m_typeString : BooleanTopic.kTypeString,
                        m_properties,
                        PubSubOption.keepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      if (pub instanceof BooleanPublisher e) {
        e.set(value);
      } else {
        // TODO: warn?
      }
    }

    @Override
    public void logLong(long value) {}

    @Override
    public void logFloat(float value) {}

    @Override
    public void logDouble(double value) {}

    @Override
    public void logString(String value) {}

    @Override
    public void logBooleanArray(boolean[] value) {}

    @Override
    public void logByteArray(byte[] value) {}

    @Override
    public void logShortArray(short[] value) {}

    @Override
    public void logIntArray(int[] value) {}

    @Override
    public void logLongArray(long[] value) {}

    @Override
    public void logFloatArray(float[] value) {}

    @Override
    public void logDoubleArray(double[] value) {}

    @Override
    public void logStringArray(String[] value) {}
  }
}
