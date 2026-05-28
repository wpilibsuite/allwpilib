// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;
import org.wpilib.networktables.BooleanArrayPublisher;
import org.wpilib.networktables.BooleanPublisher;
import org.wpilib.networktables.DoubleArrayPublisher;
import org.wpilib.networktables.DoublePublisher;
import org.wpilib.networktables.FloatArrayPublisher;
import org.wpilib.networktables.FloatPublisher;
import org.wpilib.networktables.IntegerArrayPublisher;
import org.wpilib.networktables.IntegerPublisher;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.NetworkTableType;
import org.wpilib.networktables.ProtobufPublisher;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.networktables.Publisher;
import org.wpilib.networktables.RawPublisher;
import org.wpilib.networktables.StringArrayPublisher;
import org.wpilib.networktables.StringPublisher;
import org.wpilib.networktables.StructArrayPublisher;
import org.wpilib.networktables.StructPublisher;
import org.wpilib.telemetry.TelemetryBackend;
import org.wpilib.telemetry.TelemetryEntry;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/** A telemetry backend that sends logged data to NetworkTables. */
public class NetworkTablesTelemetryBackend implements TelemetryBackend {
  private final NetworkTableInstance m_inst;
  private final String m_prefix;
  private final Map<String, Entry> m_entries = new HashMap<>();

  /**
   * Construct.
   *
   * @param inst NetworkTables instance
   * @param prefix prefix to put in front of logged path in NT
   */
  public NetworkTablesTelemetryBackend(NetworkTableInstance inst, String prefix) {
    m_inst = inst;
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
  public TelemetryEntry getEntry(String name) {
    synchronized (m_entries) {
      return m_entries.computeIfAbsent(name, k -> new Entry(m_inst, m_prefix + k));
    }
  }

  private static final class Entry implements TelemetryEntry {
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

    public void close() {
      var pub = m_pub.getAndSet(null);
      if (pub != null) {
        pub.close();
      }
    }

    @Override
    public void keepDuplicates() {
      m_keepDuplicates.set(true);
      // TODO: update publisher while not losing last value
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
          Publisher pub = m_pub.get();
          if (pub != null) {
            pub.getTopic().setProperties(m_properties);
          }
        }
      }
    }

    private synchronized <T> StructPublisher<T> initStruct(Struct<T> struct) {
      Publisher pub = m_pub.get();
      return switch (pub) {
        case null -> {
          StructPublisher<T> p =
              m_inst
                  .getStructTopic(m_path, struct)
                  .publishEx(m_properties, new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
          m_struct = struct;
          m_pub.set(p);
          yield p;
        }
        case StructPublisher<?> p when struct.equals(m_struct) -> {
          @SuppressWarnings("unchecked")
          StructPublisher<T> typedPublisher = (StructPublisher<T>) p;
          yield typedPublisher;
        }
        default -> null;
      };
    }

    @Override
    public <T> void logStruct(T value, Struct<T> struct) {
      StructPublisher<T> pub = initStruct(struct);
      if (pub != null) {
        pub.set(value);
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private synchronized <T> ProtobufPublisher<T> initProtobuf(Protobuf<T, ?> proto) {
      Publisher pub = m_pub.get();
      return switch (pub) {
        case null -> {
          ProtobufPublisher<T> p =
              m_inst
                  .getProtobufTopic(m_path, proto)
                  .publishEx(m_properties, new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
          m_proto = proto;
          m_pub.set(p);
          yield p;
        }
        case ProtobufPublisher<?> p when proto.equals(m_proto) -> {
          @SuppressWarnings("unchecked")
          ProtobufPublisher<T> typedPublisher = (ProtobufPublisher<T>) p;
          yield typedPublisher;
        }
        default -> null;
      };
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {
      ProtobufPublisher<T> pub = initProtobuf(proto);
      if (pub != null) {
        pub.set(value);
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private synchronized <T> StructArrayPublisher<T> initStructArray(Struct<T> struct) {
      Publisher pub = m_pub.get();
      return switch (pub) {
        case null -> {
          StructArrayPublisher<T> p =
              m_inst
                  .getStructArrayTopic(m_path, struct)
                  .publishEx(m_properties, new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
          m_struct = struct;
          m_pub.set(p);
          yield p;
        }
        case StructArrayPublisher<?> p when struct.equals(m_struct) -> {
          @SuppressWarnings("unchecked")
          StructArrayPublisher<T> typedPublisher = (StructArrayPublisher<T>) p;
          yield typedPublisher;
        }
        default -> null;
      };
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<T> struct) {
      StructArrayPublisher<T> pub = initStructArray(struct);
      if (pub != null) {
        pub.set(value);
      } else {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

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
                        NetworkTableType.BOOLEAN.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case BooleanPublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logLong(long value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getIntegerTopic(m_path)
                    .publishEx(
                        NetworkTableType.INTEGER.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case IntegerPublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloat(float value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getFloatTopic(m_path)
                    .publishEx(
                        NetworkTableType.FLOAT.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case FloatPublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDouble(double value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getDoubleTopic(m_path)
                    .publishEx(
                        NetworkTableType.DOUBLE.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case DoublePublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logString(String value, String typeString) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            m_typeString = typeString;
            pub =
                m_inst
                    .getStringTopic(m_path)
                    .publishEx(
                        m_typeString,
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      String curTypeString;
      synchronized (this) {
        curTypeString = m_typeString;
      }

      switch (pub) {
        case StringPublisher e when curTypeString.equals(typeString) -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logBooleanArray(boolean[] value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getBooleanArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.BOOLEAN_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case BooleanArrayPublisher e -> e.set(value);
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
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getIntegerArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.INTEGER_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case IntegerArrayPublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloatArray(float[] value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getFloatArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.FLOAT_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case FloatArrayPublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDoubleArray(double[] value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getDoubleArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.DOUBLE_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case DoubleArrayPublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logStringArray(String[] value) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            pub =
                m_inst
                    .getStringArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.STRING_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      switch (pub) {
        case StringArrayPublisher e -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logRaw(byte[] value, String typeString) {
      Publisher pub = m_pub.get();
      if (pub == null) {
        synchronized (this) {
          // double-check
          pub = m_pub.get();
          if (pub == null) {
            m_typeString = typeString;
            pub =
                m_inst
                    .getRawTopic(m_path)
                    .publishEx(
                        m_typeString,
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates.get()));
            m_pub.set(pub);
          }
        }
      }

      String curTypeString;
      synchronized (this) {
        curTypeString = m_typeString;
      }

      switch (pub) {
        case RawPublisher e when curTypeString.equals(typeString) -> e.set(value);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }
  }
}
