// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.protobuf.ProtobufBuffer;
import edu.wpi.first.util.sendable2.Sendable;
import edu.wpi.first.util.sendable2.SendableOption;
import edu.wpi.first.util.sendable2.SendableTable;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructBuffer;
import java.io.IOException;
import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** NetworkTables-backed implementation of SendableTable. */
public class DataLogSendableTable implements SendableTable {
  private static final char PATH_SEPARATOR = '/';

  private final String m_path;
  private final String m_pathWithSep;
  private final DataLog m_log;

  private enum DataType {
    kNone,
    kBoolean,
    kInteger,
    kFloat,
    kDouble,
    kString,
    kBooleanArray,
    kIntegerArray,
    kFloatArray,
    kDoubleArray,
    kStringArray,
    kRaw,
    kStruct,
    kProtobuf;
  }

  private static class EntryData {
    synchronized void close() {
      if (m_entry != 0) {
        m_log.finish(m_entry);
        m_entry = 0;
      }
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
      if (m_entry != 0) {
        m_log.setMetadata(m_entry, m_properties);
      }
    }

    void setBoolean(boolean value) {
      if (m_type != DataType.kBoolean) {
        return;
      }
      m_log.appendBoolean(m_entry, value, 0);
    }

    void setInteger(long value) {
      if (m_type != DataType.kInteger) {
        return;
      }
      m_log.appendInteger(m_entry, value, 0);
    }

    void setFloat(float value) {
      if (m_type != DataType.kFloat) {
        return;
      }
      m_log.appendFloat(m_entry, value, 0);
    }

    void setDouble(double value) {
      if (m_type != DataType.kDouble) {
        return;
      }
      m_log.appendDouble(m_entry, value, 0);
    }

    void setString(String value) {
      if (m_type != DataType.kString) {
        return;
      }
      m_log.appendString(m_entry, value, 0);
    }

    void setBooleanArray(boolean[] value) {
      if (m_type != DataType.kBooleanArray) {
        return;
      }
      m_log.appendBooleanArray(m_entry, value, 0);
    }

    void setIntegerArray(long[] value) {
      if (m_type != DataType.kIntegerArray) {
        return;
      }
      m_log.appendIntegerArray(m_entry, value, 0);
    }

    void setFloatArray(float[] value) {
      if (m_type != DataType.kFloatArray) {
        return;
      }
      m_log.appendFloatArray(m_entry, value, 0);
    }

    void setDoubleArray(double[] value) {
      if (m_type != DataType.kDoubleArray) {
        return;
      }
      m_log.appendDoubleArray(m_entry, value, 0);
    }

    void setStringArray(String[] value) {
      if (m_type != DataType.kStringArray) {
        return;
      }
      m_log.appendStringArray(m_entry, value, 0);
    }

    void setRaw(byte[] value) {
      setRaw(value, 0, value.length);
    }

    void setRaw(byte[] value, int start, int len) {
      if (m_type != DataType.kRaw && m_type != DataType.kStruct && m_type != DataType.kProtobuf) {
        return;
      }
      m_log.appendRaw(m_entry, value, start, len, 0);
    }

    void setRaw(ByteBuffer value) {
      int pos = value.position();
      setRaw(value, pos, value.limit() - pos);
    }

    void setRaw(ByteBuffer value, int start, int len) {
      if (m_type != DataType.kRaw && m_type != DataType.kStruct && m_type != DataType.kProtobuf) {
        return;
      }
      m_log.appendRaw(m_entry, value, start, len, 0);
    }

    void setPolledUpdate(Consumer<EntryData> consumer) {
      m_polledUpdate.set(consumer);
    }

    Consumer<EntryData> getPolledUpdate() {
      return m_polledUpdate.get();
    }

    DataLog m_log;
    int m_entry;
    DataType m_type = DataType.kNone;

    final Map<String, String> m_propertiesMap = new HashMap<>();
    String m_properties = "{}";
    StructBuffer<?> m_structBuffer;
    ProtobufBuffer<?, ?> m_protobufBuffer;
    final AtomicReference<Consumer<EntryData>> m_polledUpdate = new AtomicReference<>();

    // boolean m_appendAll;
  }

  private final ConcurrentMap<String, EntryData> m_entries = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, DataLogSendableTable> m_tables = new ConcurrentHashMap<>();
  private Consumer<SendableTable> m_closeSendable;
  private boolean m_closed;

  private EntryData start(String name, String typeString, DataType type) {
    EntryData data = m_entries.computeIfAbsent(name, k -> new EntryData());
    synchronized (data) {
      if (data.m_entry == 0) {
        data.m_log = m_log;
        data.m_entry = m_log.start(m_pathWithSep + name, typeString, data.m_properties, 0);
        data.m_type = type;
      }
    }
    return data;
  }

  public DataLogSendableTable(DataLog log, String path) {
    m_path = path;
    m_pathWithSep = path + PATH_SEPARATOR;
    m_log = log;
  }

  /**
   * Gets the DataLog for the table.
   *
   * @return DataLog
   */
  public DataLog getLog() {
    return m_log;
  }

  @Override
  public String toString() {
    return "DataLogSendableTable: " + m_path;
  }

  @Override
  public boolean getBoolean(String name, boolean defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public long getInteger(String name, long defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public float getFloat(String name, float defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public double getDouble(String name, double defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public String getString(String name, String defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public boolean[] getBooleanArray(String name, boolean[] defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public long[] getIntegerArray(String name, long[] defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public float[] getFloatArray(String name, float[] defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public double[] getDoubleArray(String name, double[] defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public String[] getStringArray(String name, String[] defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public byte[] getRaw(String name, String typeString, byte[] defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public <T> T getStruct(String name, Struct<T> struct, T defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public <T> boolean getStructInto(String name, T out, Struct<T> struct) {
    return false; // TODO
  }

  @Override
  public <T> T getProtobuf(String name, Protobuf<T, ?> proto, T defaultValue) {
    return defaultValue; // TODO
  }

  @Override
  public <T> boolean getProtobufInto(String name, T out, Protobuf<T, ?> proto) {
    return false; // TODO
  }

  @Override
  public void setBoolean(String name, boolean value) {
    start(name, "boolean", DataType.kBoolean).setBoolean(value);
  }

  @Override
  public void setInteger(String name, long value) {
    start(name, "int64", DataType.kInteger).setInteger(value);
  }

  @Override
  public void setFloat(String name, float value) {
    start(name, "float", DataType.kFloat).setFloat(value);
  }

  @Override
  public void setDouble(String name, double value) {
    start(name, "double", DataType.kDouble).setDouble(value);
  }

  @Override
  public void setString(String name, String value) {
    start(name, "string", DataType.kString).setString(value);
  }

  @Override
  public void setBooleanArray(String name, boolean[] value) {
    start(name, "boolean[]", DataType.kBooleanArray).setBooleanArray(value);
  }

  @Override
  public void setIntegerArray(String name, long[] value) {
    start(name, "int64[]", DataType.kIntegerArray).setIntegerArray(value);
  }

  @Override
  public void setFloatArray(String name, float[] value) {
    start(name, "float[]", DataType.kFloatArray).setFloatArray(value);
  }

  @Override
  public void setDoubleArray(String name, double[] value) {
    start(name, "double[]", DataType.kDoubleArray).setDoubleArray(value);
  }

  @Override
  public void setStringArray(String name, String[] value) {
    start(name, "string[]", DataType.kStringArray).setStringArray(value);
  }

  @Override
  public void setRaw(String name, String typeString, byte[] value, int start, int len) {
    start(name, typeString, DataType.kRaw).setRaw(value, start, len);
  }

  @Override
  public void setRaw(String name, String typeString, ByteBuffer value, int start, int len) {
    start(name, typeString, DataType.kRaw).setRaw(value, start, len);
  }

  @Override
  public <T> void setStruct(String name, T value, Struct<T> struct) {
    EntryData td = start(name, struct.getTypeString(), DataType.kStruct);
    synchronized (td) {
      if (td.m_structBuffer == null) {
        td.m_structBuffer = StructBuffer.create(struct);
        addSchema(struct);
      } else if (!td.m_structBuffer.getStruct().equals(struct)) {
        return;
      }
      @SuppressWarnings("unchecked")
      StructBuffer<T> buf = (StructBuffer<T>) td.m_structBuffer;
      ByteBuffer bb = buf.write(value);
      td.setRaw(bb, 0, bb.position());
    }
  }

  @Override
  public <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto) {
    EntryData td = start(name, proto.getTypeString(), DataType.kProtobuf);
    synchronized (td) {
      if (td.m_protobufBuffer == null) {
        td.m_protobufBuffer = ProtobufBuffer.create(proto);
        addSchema(proto);
      } else if (!td.m_protobufBuffer.getProto().equals(proto)) {
        return;
      }
      @SuppressWarnings("unchecked")
      ProtobufBuffer<T, ?> buf = (ProtobufBuffer<T, ?>) td.m_protobufBuffer;
      try {
        ByteBuffer bb = buf.write(value);
        td.setRaw(bb, 0, bb.position());
      } catch (IOException e) {
        // ignore
      }
    }
  }

  @Override
  public void publishBoolean(String name, BooleanSupplier supplier) {
    EntryData data = start(name, "boolean", DataType.kBoolean);
    data.setPolledUpdate(
        entry -> {
          entry.setBoolean(supplier.getAsBoolean());
        });
  }

  @Override
  public void publishInteger(String name, LongSupplier supplier) {
    EntryData data = start(name, "int64", DataType.kInteger);
    data.setPolledUpdate(
        entry -> {
          entry.setInteger(supplier.getAsLong());
        });
  }

  @Override
  public void publishFloat(String name, FloatSupplier supplier) {
    EntryData data = start(name, "float", DataType.kFloat);
    data.setPolledUpdate(
        entry -> {
          entry.setFloat(supplier.getAsFloat());
        });
  }

  @Override
  public void publishDouble(String name, DoubleSupplier supplier) {
    EntryData data = start(name, "double", DataType.kDouble);
    data.setPolledUpdate(
        entry -> {
          entry.setDouble(supplier.getAsDouble());
        });
  }

  @Override
  public void publishString(String name, Supplier<String> supplier) {
    EntryData data = start(name, "string", DataType.kString);
    data.setPolledUpdate(
        entry -> {
          entry.setString(supplier.get());
        });
  }

  @Override
  public void publishBooleanArray(String name, Supplier<boolean[]> supplier) {
    EntryData data = start(name, "boolean[]", DataType.kBooleanArray);
    data.setPolledUpdate(
        entry -> {
          entry.setBooleanArray(supplier.get());
        });
  }

  @Override
  public void publishIntegerArray(String name, Supplier<long[]> supplier) {
    EntryData data = start(name, "int64[]", DataType.kStringArray);
    data.setPolledUpdate(
        entry -> {
          entry.setIntegerArray(supplier.get());
        });
  }

  @Override
  public void publishFloatArray(String name, Supplier<float[]> supplier) {
    EntryData data = start(name, "float[]", DataType.kFloatArray);
    data.setPolledUpdate(
        entry -> {
          entry.setFloatArray(supplier.get());
        });
  }

  @Override
  public void publishDoubleArray(String name, Supplier<double[]> supplier) {
    EntryData data = start(name, "double[]", DataType.kDoubleArray);
    data.setPolledUpdate(
        entry -> {
          entry.setDoubleArray(supplier.get());
        });
  }

  @Override
  public void publishStringArray(String name, Supplier<String[]> supplier) {
    EntryData data = start(name, "string[]", DataType.kStringArray);
    data.setPolledUpdate(
        entry -> {
          entry.setStringArray(supplier.get());
        });
  }

  @Override
  public void publishRawBytes(String name, String typeString, Supplier<byte[]> supplier) {
    EntryData data = start(name, typeString, DataType.kRaw);
    data.setPolledUpdate(
        entry -> {
          entry.setRaw(supplier.get());
        });
  }

  @Override
  public void publishRawBuffer(String name, String typeString, Supplier<ByteBuffer> supplier) {
    EntryData data = start(name, typeString, DataType.kRaw);
    data.setPolledUpdate(
        entry -> {
          entry.setRaw(supplier.get());
        });
  }

  @Override
  public <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    EntryData td = start(name, struct.getTypeString(), DataType.kStruct);
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    td.setPolledUpdate(
        entry -> {
          entry.setRaw(buf.write(supplier.get()));
        });
  }

  @Override
  public <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier) {
    EntryData td = start(name, proto.getTypeString(), DataType.kProtobuf);
    addSchema(proto);
    ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    td.setPolledUpdate(
        entry -> {
          try {
            entry.setRaw(buf.write(supplier.get()));
          } catch (IOException e) {
            return; // ignore
          }
        });
  }

  @Override
  public BooleanConsumer addBooleanPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "boolean", DataType.kBoolean));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setBoolean(value);
      }
    };
  }

  @Override
  public LongConsumer addIntegerPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "int64", DataType.kInteger));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setInteger(value);
      }
    };
  }

  @Override
  public FloatConsumer addFloatPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "float", DataType.kFloat));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setFloat(value);
      }
    };
  }

  @Override
  public DoubleConsumer addDoublePublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "double", DataType.kDouble));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setDouble(value);
      }
    };
  }

  @Override
  public Consumer<String> addStringPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "string", DataType.kString));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setString(value);
      }
    };
  }

  @Override
  public Consumer<boolean[]> addBooleanArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "boolean[]", DataType.kBooleanArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setBooleanArray(value);
      }
    };
  }

  @Override
  public Consumer<long[]> addIntegerArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "int64[]", DataType.kIntegerArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setIntegerArray(value);
      }
    };
  }

  @Override
  public Consumer<float[]> addFloatArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "float[]", DataType.kFloatArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setFloatArray(value);
      }
    };
  }

  @Override
  public Consumer<double[]> addDoubleArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "double[]", DataType.kDoubleArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setDoubleArray(value);
      }
    };
  }

  @Override
  public Consumer<String[]> addStringArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, "string[]", DataType.kStringArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setStringArray(value);
      }
    };
  }

  @Override
  public Consumer<byte[]> addRawBytesPublisher(String name, String typeString) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, typeString, DataType.kRaw));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setRaw(value);
      }
    };
  }

  @Override
  public Consumer<ByteBuffer> addRawBufferPublisher(String name, String typeString) {
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, typeString, DataType.kRaw));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setRaw(value);
      }
    };
  }

  @Override
  public <T> Consumer<T> addStructPublisher(String name, Struct<T> struct) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, struct.getTypeString(), DataType.kStruct));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        data.setRaw(buf.write(value));
      }
    };
  }

  @Override
  public <T> Consumer<T> addProtobufPublisher(String name, Protobuf<T, ?> proto) {
    addSchema(proto);
    final ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    final WeakReference<EntryData> dataRef =
        new WeakReference<>(start(name, proto.getTypeString(), DataType.kProtobuf));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.m_entry != 0) {
        try {
          data.setRaw(buf.write(value));
        } catch (IOException e) {
          // ignore
        }
      }
    };
  }

  @Override
  public void subscribeBoolean(String name, BooleanConsumer consumer) {}

  @Override
  public void subscribeInteger(String name, LongConsumer consumer) {}

  @Override
  public void subscribeFloat(String name, FloatConsumer consumer) {}

  @Override
  public void subscribeDouble(String name, DoubleConsumer consumer) {}

  @Override
  public void subscribeString(String name, Consumer<String> consumer) {}

  @Override
  public void subscribeBooleanArray(String name, Consumer<boolean[]> consumer) {}

  @Override
  public void subscribeIntegerArray(String name, Consumer<long[]> consumer) {}

  @Override
  public void subscribeFloatArray(String name, Consumer<float[]> consumer) {}

  @Override
  public void subscribeDoubleArray(String name, Consumer<double[]> consumer) {}

  @Override
  public void subscribeStringArray(String name, Consumer<String[]> consumer) {}

  @Override
  public void subscribeRawBytes(String name, String typeString, Consumer<byte[]> consumer) {}

  @Override
  public <T> DataLogSendableTable addSendable(String name, T obj, Sendable<T> sendable) {
    DataLogSendableTable child = getChild(name);
    if (child.m_closeSendable == null) {
      sendable.initSendable(obj, child);
      child.m_closeSendable =
          table -> {
            sendable.closeSendable(obj, table);
          };
    }
    return child;
  }

  @SuppressWarnings("resource")
  @Override
  public DataLogSendableTable getChild(String name) {
    return m_tables.computeIfAbsent(
        name, k -> new DataLogSendableTable(m_log, m_pathWithSep + name));
  }

  @Override
  public void setPublishOptions(String name, SendableOption... options) {
    // TODO
  }

  @Override
  public void setSubscribeOptions(String name, SendableOption... options) {}

  /**
   * Gets the current value of a property (as a JSON string).
   *
   * @param name name
   * @param propName property name
   * @return JSON string; "null" if the property does not exist.
   */
  @Override
  public String getProperty(String name, String propName) {
    EntryData data = m_entries.get(name);
    if (data == null) {
      return "null";
    }
    String value = data.m_propertiesMap.get(propName);
    return value != null ? value : "null";
  }

  /**
   * Sets a property value.
   *
   * @param name name
   * @param propName property name
   * @param value property value (JSON string)
   * @throws IllegalArgumentException if properties is not parseable as JSON
   */
  @Override
  public void setProperty(String name, String propName, String value) {
    EntryData data = m_entries.computeIfAbsent(name, k -> new EntryData());
    synchronized (data) {
      data.m_propertiesMap.put(propName, value);
      data.refreshProperties();
    }
  }

  /**
   * Deletes a property. Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  @Override
  public void deleteProperty(String name, String propName) {
    EntryData data = m_entries.get(name);
    if (data == null) {
      return;
    }
    synchronized (data) {
      if (data.m_propertiesMap.remove(propName) != null) {
        data.refreshProperties();
      }
    }
  }

  /**
   * Gets all topic properties as a JSON object string. Each key in the object is the property name,
   * and the corresponding value is the property value.
   *
   * @param name name
   * @return JSON string
   */
  @Override
  public String getProperties(String name) {
    EntryData data = m_entries.get(name);
    if (data == null) {
      return "{}";
    }
    synchronized (data) {
      return data.m_properties;
    }
  }

  /**
   * Updates multiple topic properties. Each key in the passed-in object is the name of the property
   * to add/update, and the corresponding value is the property value to set for that property. Null
   * values result in deletion of the corresponding property.
   *
   * @param name name
   * @param properties map of keys/JSON values to add/update/delete
   * @throws IllegalArgumentException if properties is not a JSON object
   */
  @Override
  public void setProperties(String name, Map<String, String> properties) {
    if (properties.isEmpty()) {
      return;
    }
    EntryData data = m_entries.computeIfAbsent(name, k -> new EntryData());
    synchronized (data) {
      properties.forEach(
          (k, v) -> {
            if (v == null) {
              data.m_propertiesMap.remove(k);
            } else {
              data.m_propertiesMap.put(k, v);
            }
          });
      data.refreshProperties();
    }
  }

  @Override
  public void remove(String name) {
    EntryData data = m_entries.remove(name);
    if (data != null) {
      data.close();
    }
  }

  @Override
  public void addPeriodic(Runnable runnable) {
    // TODO
  }

  /**
   * Return whether this sendable has been published.
   *
   * @return True if it has been published, false if not.
   */
  @Override
  public boolean isPublished() {
    return true;
  }

  /** Update the published values by calling the getters for all properties. */
  @Override
  public void update() {
    for (EntryData data : m_entries.values()) {
      Consumer<EntryData> consumer = data.getPolledUpdate();
      if (consumer != null && data.m_entry != 0) {
        consumer.accept(data);
      }
    }
    for (DataLogSendableTable table : m_tables.values()) {
      table.update();
    }
  }

  /** Erases all publishers and subscribers. */
  @Override
  public void clear() {
    for (EntryData data : m_entries.values()) {
      data.close();
    }
    m_entries.clear();
    for (DataLogSendableTable table : m_tables.values()) {
      table.close();
    }
    m_tables.clear();
  }

  /**
   * Returns whether there is a data schema already registered with the given name that this
   * instance has published. This does NOT perform a check as to whether the schema has already been
   * published by another node on the network.
   *
   * @param name Name (the string passed as the data type for topics using this schema)
   * @return True if schema already registered
   */
  @Override
  public boolean hasSchema(String name) {
    return m_log.hasSchema(name);
  }

  /**
   * Registers a data schema. Data schemas provide information for how a certain data type string
   * can be decoded. The type string of a data schema indicates the type of the schema itself (e.g.
   * "protobuf" for protobuf schemas, "struct" for struct schemas, etc). In NetworkTables, schemas
   * are published just like normal topics, with the name being generated from the provided name:
   * "/.schema/name". Duplicate calls to this function with the same name are silently ignored.
   *
   * @param name Name (the string passed as the data type for topics using this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  @Override
  public void addSchema(String name, String type, byte[] schema) {
    m_log.addSchema(name, type, schema);
  }

  /**
   * Registers a data schema. Data schemas provide information for how a certain data type string
   * can be decoded. The type string of a data schema indicates the type of the schema itself (e.g.
   * "protobuf" for protobuf schemas, "struct" for struct schemas, etc). In NetworkTables, schemas
   * are published just like normal topics, with the name being generated from the provided name:
   * "/.schema/name". Duplicate calls to this function with the same name are silently ignored.
   *
   * @param name Name (the string passed as the data type for topics using this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  @Override
  public void addSchema(String name, String type, String schema) {
    m_log.addSchema(name, type, schema);
  }

  /**
   * Registers a protobuf schema. Duplicate calls to this function with the same name are silently
   * ignored.
   *
   * @param proto protobuf serialization object
   */
  @Override
  public void addSchema(Protobuf<?, ?> proto) {
    m_log.addSchema(proto);
  }

  /**
   * Registers a struct schema. Duplicate calls to this function with the same name are silently
   * ignored.
   *
   * @param struct struct serialization object
   */
  @Override
  public void addSchema(Struct<?> struct) {
    m_log.addSchema(struct);
  }

  /**
   * Return whether close() has been called on this sendable table.
   *
   * @return True if closed, false otherwise.
   */
  @Override
  public boolean isClosed() {
    return m_closed;
  }

  @Override
  public void close() {
    clear();
    if (m_closeSendable != null) {
      m_closeSendable.accept(this);
      m_closeSendable = null;
    }
    m_closed = true;
  }
}
