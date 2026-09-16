// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import io.avaje.json.JsonException;
import io.avaje.jsonb.JsonType;
import io.avaje.jsonb.Jsonb;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;
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
import org.wpilib.networktables.PubSubOption;
import org.wpilib.networktables.Publisher;
import org.wpilib.networktables.RawPublisher;
import org.wpilib.networktables.StringArrayPublisher;
import org.wpilib.networktables.StringPublisher;
import org.wpilib.telemetry.TelemetryBackend;
import org.wpilib.telemetry.TelemetryEntry;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufBuffer;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructBuffer;

/** A telemetry backend that sends logged data to NetworkTables. */
public class NetworkTablesTelemetryBackend implements TelemetryBackend {
  private static final Jsonb s_json = Jsonb.instance();
  private static final JsonType<Object> s_jsonValue = s_json.type(Object.class);

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
    private final NetworkTableInstance m_inst;
    private final String m_path;
    private volatile Publisher m_pub;
    private volatile boolean m_closed;
    private volatile String m_typeString;
    private volatile boolean m_keepDuplicates;
    private final Map<String, String> m_propertiesMap = new HashMap<>();
    private String m_properties = "{}";
    private Struct<?> m_struct;
    private StructBuffer<?> m_structBuffer;
    private Protobuf<?, ?> m_proto;
    private ProtobufBuffer<?, ?> m_protoBuffer;

    Entry(NetworkTableInstance inst, String path) {
      m_inst = inst;
      m_path = path;
    }

    public void close() {
      Publisher pub;
      synchronized (this) {
        pub = m_pub;
        m_pub = null;
        m_closed = true;
      }
      if (pub != null) {
        pub.close();
      }
    }

    @Override
    public boolean isDiscard() {
      return m_closed;
    }

    @Override
    public void keepDuplicates() {
      Publisher oldPub = null;
      synchronized (this) {
        if (m_closed) {
          return;
        }
        if (m_keepDuplicates) {
          return;
        }
        m_keepDuplicates = true;

        Publisher pub = m_pub;
        if (pub != null) {
          Publisher newPub = republish(pub);
          if (newPub != null && newPub.isValid()) {
            m_pub = newPub;
            oldPub = pub;
          } else if (newPub != null) {
            newPub.close();
          }
        }
      }
      if (oldPub != null) {
        oldPub.close();
      }
    }

    private Publisher republish(Publisher pub) {
      return switch (pub) {
        case BooleanPublisher ignored ->
            m_inst
                .getBooleanTopic(m_path)
                .publishEx(
                    NetworkTableType.BOOLEAN.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case IntegerPublisher ignored ->
            m_inst
                .getIntegerTopic(m_path)
                .publishEx(
                    NetworkTableType.INTEGER.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case FloatPublisher ignored ->
            m_inst
                .getFloatTopic(m_path)
                .publishEx(
                    NetworkTableType.FLOAT.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case DoublePublisher ignored ->
            m_inst
                .getDoubleTopic(m_path)
                .publishEx(
                    NetworkTableType.DOUBLE.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case StringPublisher ignored ->
            m_inst
                .getStringTopic(m_path)
                .publishEx(m_typeString, m_properties, PubSubOption.KEEP_DUPLICATES);
        case BooleanArrayPublisher ignored ->
            m_inst
                .getBooleanArrayTopic(m_path)
                .publishEx(
                    NetworkTableType.BOOLEAN_ARRAY.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case IntegerArrayPublisher ignored ->
            m_inst
                .getIntegerArrayTopic(m_path)
                .publishEx(
                    NetworkTableType.INTEGER_ARRAY.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case FloatArrayPublisher ignored ->
            m_inst
                .getFloatArrayTopic(m_path)
                .publishEx(
                    NetworkTableType.FLOAT_ARRAY.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case DoubleArrayPublisher ignored ->
            m_inst
                .getDoubleArrayTopic(m_path)
                .publishEx(
                    NetworkTableType.DOUBLE_ARRAY.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case StringArrayPublisher ignored ->
            m_inst
                .getStringArrayTopic(m_path)
                .publishEx(
                    NetworkTableType.STRING_ARRAY.getValueStr(),
                    m_properties,
                    PubSubOption.KEEP_DUPLICATES);
        case RawPublisher ignored ->
            m_inst
                .getRawTopic(m_path)
                .publishEx(m_typeString, m_properties, PubSubOption.KEEP_DUPLICATES);
        default -> null;
      };
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
          Publisher pub = m_pub;
          if (pub != null) {
            pub.getTopic().setProperties(m_properties);
          }
        }
      }
    }

    private <T> RawPublisher initStruct(Struct<T> struct, boolean array) {
      if (m_closed) {
        return null;
      }
      String typeString = array ? struct.getTypeString() + "[]" : struct.getTypeString();
      Publisher pub = m_pub;
      return switch (pub) {
        case null -> {
          m_inst.addSchema(struct);
          final RawPublisher p =
              m_inst
                  .getRawTopic(m_path)
                  .publishEx(
                      typeString, m_properties, new PubSubOption.KeepDuplicates(m_keepDuplicates));
          m_struct = struct;
          m_structBuffer = StructBuffer.create(struct);
          m_typeString = typeString;
          m_pub = p;
          yield p;
        }
        case RawPublisher p when typeString.equals(m_typeString) && struct.equals(m_struct) -> p;
        default -> null;
      };
    }

    @Override
    public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {
      boolean typeMismatch = false;
      try {
        synchronized (this) {
          RawPublisher pub = initStruct(struct, false);
          if (pub != null) {
            @SuppressWarnings("unchecked")
            StructBuffer<T> buffer = (StructBuffer<T>) m_structBuffer;
            var bb = buffer.write(value);
            pub.set(bb, 0, bb.position(), timestamp);
          } else if (!m_closed) {
            typeMismatch = true;
          }
        }
      } catch (RuntimeException e) {
        reportWarning("failed to publish struct value", e);
        return;
      }
      if (typeMismatch) {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private <T> RawPublisher initProtobuf(Protobuf<T, ?> proto) {
      if (m_closed) {
        return null;
      }
      String typeString = proto.getTypeString();
      Publisher pub = m_pub;
      return switch (pub) {
        case null -> {
          m_inst.addSchema(proto);
          final RawPublisher p =
              m_inst
                  .getRawTopic(m_path)
                  .publishEx(
                      typeString, m_properties, new PubSubOption.KeepDuplicates(m_keepDuplicates));
          m_proto = proto;
          m_protoBuffer = ProtobufBuffer.create(proto);
          m_typeString = typeString;
          m_pub = p;
          yield p;
        }
        case RawPublisher p when typeString.equals(m_typeString) && proto.equals(m_proto) -> p;
        default -> null;
      };
    }

    @Override
    public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {
      boolean typeMismatch = false;
      try {
        synchronized (this) {
          RawPublisher pub = initProtobuf(proto);
          if (pub != null) {
            @SuppressWarnings("unchecked")
            ProtobufBuffer<T, ?> buffer = (ProtobufBuffer<T, ?>) m_protoBuffer;
            var bb = buffer.write(value);
            pub.set(bb, 0, bb.position(), timestamp);
          } else if (!m_closed) {
            typeMismatch = true;
          }
        }
      } catch (IOException | RuntimeException e) {
        reportWarning("failed to publish protobuf value", e);
        return;
      }
      if (typeMismatch) {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {
      boolean typeMismatch = false;
      try {
        synchronized (this) {
          RawPublisher pub = initStruct(struct, true);
          if (pub != null) {
            @SuppressWarnings("unchecked")
            StructBuffer<T> buffer = (StructBuffer<T>) m_structBuffer;
            var bb = buffer.writeArray(value);
            pub.set(bb, 0, bb.position(), timestamp);
          } else if (!m_closed) {
            typeMismatch = true;
          }
        }
      } catch (RuntimeException e) {
        reportWarning("failed to publish struct array value", e);
        return;
      }
      if (typeMismatch) {
        TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    private void reportWarning(String msg, Exception exception) {
      String exceptionMessage = exception.getMessage();
      TelemetryRegistry.reportWarning(
          m_path, exceptionMessage == null ? msg : msg + ": " + exceptionMessage);
    }

    @Override
    public void logBoolean(boolean value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getBooleanTopic(m_path)
                    .publishEx(
                        NetworkTableType.BOOLEAN.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case BooleanPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logLong(long value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getIntegerTopic(m_path)
                    .publishEx(
                        NetworkTableType.INTEGER.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case IntegerPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloat(float value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getFloatTopic(m_path)
                    .publishEx(
                        NetworkTableType.FLOAT.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case FloatPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDouble(double value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getDoubleTopic(m_path)
                    .publishEx(
                        NetworkTableType.DOUBLE.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case DoublePublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logString(String value, String typeString, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            m_typeString = typeString;
            pub =
                m_inst
                    .getStringTopic(m_path)
                    .publishEx(
                        m_typeString,
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      String curTypeString = m_typeString;

      switch (pub) {
        case StringPublisher e when curTypeString.equals(typeString) -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logBooleanArray(boolean[] value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getBooleanArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.BOOLEAN_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case BooleanArrayPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logShortArray(short[] value, long timestamp) {
      long[] arr = new long[value.length];
      for (int i = 0; i < value.length; i++) {
        arr[i] = value[i];
      }
      logLongArray(arr, timestamp);
    }

    @Override
    public void logIntArray(int[] value, long timestamp) {
      long[] arr = new long[value.length];
      for (int i = 0; i < value.length; i++) {
        arr[i] = value[i];
      }
      logLongArray(arr, timestamp);
    }

    @Override
    public void logLongArray(long[] value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getIntegerArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.INTEGER_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case IntegerArrayPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logFloatArray(float[] value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getFloatArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.FLOAT_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case FloatArrayPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logDoubleArray(double[] value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getDoubleArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.DOUBLE_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case DoubleArrayPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logStringArray(String[] value, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            pub =
                m_inst
                    .getStringArrayTopic(m_path)
                    .publishEx(
                        NetworkTableType.STRING_ARRAY.getValueStr(),
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      switch (pub) {
        case StringArrayPublisher e -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }

    @Override
    public void logRaw(byte[] value, String typeString, long timestamp) {
      Publisher pub = m_pub;
      if (pub == null) {
        synchronized (this) {
          if (m_closed) {
            return;
          }
          // double-check
          pub = m_pub;
          if (pub == null) {
            m_typeString = typeString;
            pub =
                m_inst
                    .getRawTopic(m_path)
                    .publishEx(
                        m_typeString,
                        m_properties,
                        new PubSubOption.KeepDuplicates(m_keepDuplicates));
            m_pub = pub;
          }
        }
      }

      String curTypeString = m_typeString;

      switch (pub) {
        case RawPublisher e when curTypeString.equals(typeString) -> e.set(value, timestamp);
        default -> TelemetryRegistry.reportWarning(m_path, "type mismatch");
      }
    }
  }
}
