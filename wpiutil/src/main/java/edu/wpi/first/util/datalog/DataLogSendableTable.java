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

/** DataLog-backed implementation of SendableTable. */
public class DataLogSendableTable implements SendableTable {
  private static final char PATH_SEPARATOR = '/';

  private final String m_path;
  private final String m_pathWithSep;
  private final DataLog m_log;

  private static class EntryData {
    EntryData(DataLog log, String path) {
      m_log = log;
      m_path = path;
    }

    synchronized void close() {
      if (m_entry != null) {
        m_entry.finish();
        m_entry = null;
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
      if (m_entry != null) {
        m_entry.setMetadata(m_properties);
      }
    }

    synchronized BooleanLogEntry initBoolean() {
      if (m_entry == null) {
        BooleanLogEntry entry = new BooleanLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof BooleanLogEntry) {
        return (BooleanLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized IntegerLogEntry initInteger() {
      if (m_entry == null) {
        IntegerLogEntry entry = new IntegerLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof IntegerLogEntry) {
        return (IntegerLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized FloatLogEntry initFloat() {
      if (m_entry == null) {
        FloatLogEntry entry = new FloatLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof FloatLogEntry) {
        return (FloatLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized DoubleLogEntry initDouble() {
      if (m_entry == null) {
        DoubleLogEntry entry = new DoubleLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof DoubleLogEntry) {
        return (DoubleLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized StringLogEntry initString() {
      if (m_entry == null) {
        StringLogEntry entry = new StringLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof StringLogEntry) {
        return (StringLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized BooleanArrayLogEntry initBooleanArray() {
      if (m_entry == null) {
        BooleanArrayLogEntry entry = new BooleanArrayLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof BooleanArrayLogEntry) {
        return (BooleanArrayLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized IntegerArrayLogEntry initIntegerArray() {
      if (m_entry == null) {
        IntegerArrayLogEntry entry = new IntegerArrayLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof IntegerArrayLogEntry) {
        return (IntegerArrayLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized FloatArrayLogEntry initFloatArray() {
      if (m_entry == null) {
        FloatArrayLogEntry entry = new FloatArrayLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof FloatArrayLogEntry) {
        return (FloatArrayLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized DoubleArrayLogEntry initDoubleArray() {
      if (m_entry == null) {
        DoubleArrayLogEntry entry = new DoubleArrayLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof DoubleArrayLogEntry) {
        return (DoubleArrayLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized StringArrayLogEntry initStringArray() {
      if (m_entry == null) {
        StringArrayLogEntry entry = new StringArrayLogEntry(m_log, m_path, m_properties);
        m_entry = entry;
        return entry;
      } else if (m_entry instanceof StringArrayLogEntry) {
        return (StringArrayLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized RawLogEntry initRaw(String typeString) {
      if (m_entry == null) {
        RawLogEntry entry = new RawLogEntry(m_log, m_path, m_properties, typeString);
        m_entry = entry;
        m_typeString = typeString;
        return entry;
      } else if (m_entry instanceof RawLogEntry && typeString.equals(m_typeString)) {
        return (RawLogEntry) m_entry;
      } else {
        return null;
      }
    }

    synchronized <T> StructLogEntry<T> initStruct(Struct<T> struct) {
      if (m_entry == null) {
        StructLogEntry<T> entry = StructLogEntry.create(m_log, m_path, struct, m_properties);
        m_entry = entry;
        m_structBuffer = StructBuffer.create(struct);
        m_log.addSchema(struct);
        return entry;
      } else if (m_structBuffer != null && m_structBuffer.getStruct().equals(struct)) {
        @SuppressWarnings("unchecked")
        StructLogEntry<T> entry = (StructLogEntry<T>) m_entry;
        return entry;
      } else {
        return null;
      }
    }

    synchronized <T> ProtobufLogEntry<T> initProtobuf(Protobuf<T, ?> proto) {
      if (m_entry == null) {
        ProtobufLogEntry<T> entry = ProtobufLogEntry.create(m_log, m_path, proto, m_properties);
        m_entry = entry;
        m_protobufBuffer = ProtobufBuffer.create(proto);
        m_log.addSchema(proto);
        return entry;
      } else if (m_protobufBuffer != null && m_protobufBuffer.getProto().equals(proto)) {
        @SuppressWarnings("unchecked")
        ProtobufLogEntry<T> entry = (ProtobufLogEntry<T>) m_entry;
        return entry;
      } else {
        return null;
      }
    }

    void setBoolean(boolean value) {
      BooleanLogEntry entry = (BooleanLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setInteger(long value) {
      IntegerLogEntry entry = (IntegerLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setFloat(float value) {
      FloatLogEntry entry = (FloatLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setDouble(double value) {
      DoubleLogEntry entry = (DoubleLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setString(String value) {
      StringLogEntry entry = (StringLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setBooleanArray(boolean[] value) {
      BooleanArrayLogEntry entry = (BooleanArrayLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setIntegerArray(long[] value) {
      IntegerArrayLogEntry entry = (IntegerArrayLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setFloatArray(float[] value) {
      FloatArrayLogEntry entry = (FloatArrayLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setDoubleArray(double[] value) {
      DoubleArrayLogEntry entry = (DoubleArrayLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setStringArray(String[] value) {
      StringArrayLogEntry entry = (StringArrayLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value);
      } else {
        entry.update(value);
      }
    }

    void setRaw(byte[] value) {
      setRaw(value, 0, value.length);
    }

    void setRaw(byte[] value, int start, int len) {
      RawLogEntry entry = (RawLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value, start, len);
      } else {
        entry.update(value, start, len);
      }
    }

    void setRaw(ByteBuffer value) {
      int pos = value.position();
      setRaw(value, pos, value.limit() - pos);
    }

    void setRaw(ByteBuffer value, int start, int len) {
      RawLogEntry entry = (RawLogEntry) m_entry;
      if (m_appendAll) {
        entry.append(value, start, len);
      } else {
        entry.update(value, start, len);
      }
    }

    void setPolledUpdate(Consumer<EntryData> consumer) {
      m_polledUpdate.set(consumer);
    }

    Consumer<EntryData> getPolledUpdate() {
      return m_polledUpdate.get();
    }

    final DataLog m_log;
    final String m_path;
    final Map<String, String> m_propertiesMap = new HashMap<>();
    final AtomicReference<Consumer<EntryData>> m_polledUpdate = new AtomicReference<>();

    DataLogEntry m_entry;
    String m_typeString;

    String m_properties = "{}";
    StructBuffer<?> m_structBuffer;
    ProtobufBuffer<?, ?> m_protobufBuffer;

    boolean m_appendAll;
  }

  private final ConcurrentMap<String, EntryData> m_entries = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, DataLogSendableTable> m_tables = new ConcurrentHashMap<>();
  private Consumer<SendableTable> m_closeSendable;
  private boolean m_closed;

  private <T extends DataLogEntry> T get(String name, Class<T> cls) {
    EntryData data = m_entries.get(name);
    if (data == null || data.m_entry == null || data.m_entry.getClass() != cls) {
      return null;
    }
    @SuppressWarnings("unchecked")
    T entry = (T) data.m_entry;
    return entry;
  }

  private EntryData getOrNew(String name) {
    return m_entries.computeIfAbsent(name, k -> new EntryData(m_log, m_pathWithSep + name));
  }

  /**
   * Constructs a DataLog-backed sendable table.
   *
   * @param log DataLog
   * @param path path to sendable table, excluding trailing '/'
   */
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
    BooleanLogEntry entry = get(name, BooleanLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public long getInteger(String name, long defaultValue) {
    IntegerLogEntry entry = get(name, IntegerLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public float getFloat(String name, float defaultValue) {
    FloatLogEntry entry = get(name, FloatLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public double getDouble(String name, double defaultValue) {
    DoubleLogEntry entry = get(name, DoubleLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public String getString(String name, String defaultValue) {
    StringLogEntry entry = get(name, StringLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public boolean[] getBooleanArray(String name, boolean[] defaultValue) {
    BooleanArrayLogEntry entry = get(name, BooleanArrayLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public long[] getIntegerArray(String name, long[] defaultValue) {
    IntegerArrayLogEntry entry = get(name, IntegerArrayLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public float[] getFloatArray(String name, float[] defaultValue) {
    FloatArrayLogEntry entry = get(name, FloatArrayLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public double[] getDoubleArray(String name, double[] defaultValue) {
    DoubleArrayLogEntry entry = get(name, DoubleArrayLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public String[] getStringArray(String name, String[] defaultValue) {
    StringArrayLogEntry entry = get(name, StringArrayLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public byte[] getRaw(String name, String typeString, byte[] defaultValue) {
    RawLogEntry entry = get(name, RawLogEntry.class);
    if (entry == null) {
      return defaultValue;
    }
    return entry.getLastValue();
  }

  @Override
  public <T> T getStruct(String name, Struct<T> struct, T defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.m_structBuffer == null || !data.m_structBuffer.getStruct().equals(struct)) {
      return defaultValue;
    }
    @SuppressWarnings("unchecked")
    StructLogEntry<T> entry = (StructLogEntry<T>) data.m_entry;
    return entry.getLastValue();
  }

  @Override
  public <T> boolean getStructInto(String name, T out, Struct<T> struct) {
    return false; // TODO
  }

  @Override
  public <T> T getProtobuf(String name, Protobuf<T, ?> proto, T defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.m_protobufBuffer == null || !data.m_protobufBuffer.getProto().equals(proto)) {
      return defaultValue;
    }
    @SuppressWarnings("unchecked")
    ProtobufLogEntry<T> entry = (ProtobufLogEntry<T>) data.m_entry;
    return entry.getLastValue();
  }

  @Override
  public <T> boolean getProtobufInto(String name, T out, Protobuf<T, ?> proto) {
    return false; // TODO
  }

  @Override
  public void setBoolean(String name, boolean value) {
    EntryData data = getOrNew(name);
    if (data.initBoolean() != null) {
      data.setBoolean(value);
    }
  }

  @Override
  public void setInteger(String name, long value) {
    EntryData data = getOrNew(name);
    if (data.initInteger() != null) {
      data.setInteger(value);
    }
  }

  @Override
  public void setFloat(String name, float value) {
    EntryData data = getOrNew(name);
    if (data.initFloat() != null) {
      data.setFloat(value);
    }
  }

  @Override
  public void setDouble(String name, double value) {
    EntryData data = getOrNew(name);
    if (data.initDouble() != null) {
      data.setDouble(value);
    }
  }

  @Override
  public void setString(String name, String value) {
    EntryData data = getOrNew(name);
    if (data.initString() != null) {
      data.setString(value);
    }
  }

  @Override
  public void setBooleanArray(String name, boolean[] value) {
    EntryData data = getOrNew(name);
    if (data.initBooleanArray() != null) {
      data.setBooleanArray(value);
    }
  }

  @Override
  public void setIntegerArray(String name, long[] value) {
    EntryData data = getOrNew(name);
    if (data.initIntegerArray() != null) {
      data.setIntegerArray(value);
    }
  }

  @Override
  public void setFloatArray(String name, float[] value) {
    EntryData data = getOrNew(name);
    if (data.initFloatArray() != null) {
      data.setFloatArray(value);
    }
  }

  @Override
  public void setDoubleArray(String name, double[] value) {
    EntryData data = getOrNew(name);
    if (data.initDoubleArray() != null) {
      data.setDoubleArray(value);
    }
  }

  @Override
  public void setStringArray(String name, String[] value) {
    EntryData data = getOrNew(name);
    if (data.initStringArray() != null) {
      data.setStringArray(value);
    }
  }

  @Override
  public void setRaw(String name, String typeString, byte[] value, int start, int len) {
    EntryData data = getOrNew(name);
    if (data.initRaw(typeString) != null) {
      data.setRaw(value, start, len);
    }
  }

  @Override
  public void setRaw(String name, String typeString, ByteBuffer value, int start, int len) {
    EntryData data = getOrNew(name);
    if (data.initRaw(typeString) != null) {
      data.setRaw(value, start, len);
    }
  }

  @Override
  public <T> void setStruct(String name, T value, Struct<T> struct) {
    EntryData data = getOrNew(name);
    if (data.initStruct(struct) != null) {
      synchronized (data) {
        @SuppressWarnings("unchecked")
        StructBuffer<T> buf = (StructBuffer<T>) data.m_structBuffer;
        ByteBuffer bb = buf.write(value);
        data.setRaw(bb, 0, bb.position());
      }
    }
  }

  @Override
  public <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto) {
    EntryData data = getOrNew(name);
    if (data.initProtobuf(proto) != null) {
      synchronized (data) {
        @SuppressWarnings("unchecked")
        ProtobufBuffer<T, ?> buf = (ProtobufBuffer<T, ?>) data.m_protobufBuffer;
        try {
          ByteBuffer bb = buf.write(value);
          data.setRaw(bb, 0, bb.position());
        } catch (IOException e) {
          // ignore
        }
      }
    }
  }

  @Override
  public void publishBoolean(String name, BooleanSupplier supplier) {
    EntryData data = getOrNew(name);
    if (data.initBoolean() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setBoolean(supplier.getAsBoolean());
          });
    }
  }

  @Override
  public void publishInteger(String name, LongSupplier supplier) {
    EntryData data = getOrNew(name);
    if (data.initInteger() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setInteger(supplier.getAsLong());
          });
    }
  }

  @Override
  public void publishFloat(String name, FloatSupplier supplier) {
    EntryData data = getOrNew(name);
    if (data.initFloat() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setFloat(supplier.getAsFloat());
          });
    }
  }

  @Override
  public void publishDouble(String name, DoubleSupplier supplier) {
    EntryData data = getOrNew(name);
    if (data.initDouble() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setDouble(supplier.getAsDouble());
          });
    }
  }

  @Override
  public void publishString(String name, Supplier<String> supplier) {
    EntryData data = getOrNew(name);
    if (data.initString() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setString(supplier.get());
          });
    }
  }

  @Override
  public void publishBooleanArray(String name, Supplier<boolean[]> supplier) {
    EntryData data = getOrNew(name);
    if (data.initBooleanArray() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setBooleanArray(supplier.get());
          });
    }
  }

  @Override
  public void publishIntegerArray(String name, Supplier<long[]> supplier) {
    EntryData data = getOrNew(name);
    if (data.initIntegerArray() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setIntegerArray(supplier.get());
          });
    }
  }

  @Override
  public void publishFloatArray(String name, Supplier<float[]> supplier) {
    EntryData data = getOrNew(name);
    if (data.initFloatArray() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setFloatArray(supplier.get());
          });
    }
  }

  @Override
  public void publishDoubleArray(String name, Supplier<double[]> supplier) {
    EntryData data = getOrNew(name);
    if (data.initDoubleArray() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setDoubleArray(supplier.get());
          });
    }
  }

  @Override
  public void publishStringArray(String name, Supplier<String[]> supplier) {
    EntryData data = getOrNew(name);
    if (data.initStringArray() != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setStringArray(supplier.get());
          });
    }
  }

  @Override
  public void publishRawBytes(String name, String typeString, Supplier<byte[]> supplier) {
    EntryData data = getOrNew(name);
    if (data.initRaw(typeString) != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setRaw(supplier.get());
          });
    }
  }

  @Override
  public void publishRawBuffer(String name, String typeString, Supplier<ByteBuffer> supplier) {
    EntryData data = getOrNew(name);
    if (data.initRaw(typeString) != null) {
      data.setPolledUpdate(
          entry -> {
            entry.setRaw(supplier.get());
          });
    }
  }

  @Override
  public <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    EntryData data = getOrNew(name);
    if (data.initRaw(struct.getTypeString()) != null) {
      addSchema(struct);
      final StructBuffer<T> buf = StructBuffer.create(struct);
      data.setPolledUpdate(
          entry -> {
            entry.setRaw(buf.write(supplier.get()));
          });
    }
  }

  @Override
  public <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier) {
    EntryData data = getOrNew(name);
    if (data.initRaw(proto.getTypeString()) != null) {
      addSchema(proto);
      ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
      data.setPolledUpdate(
          entry -> {
            try {
              entry.setRaw(buf.write(supplier.get()));
            } catch (IOException e) {
              return; // ignore
            }
          });
    }
  }

  @Override
  public BooleanConsumer addBooleanPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initBoolean() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setBoolean(value);
      }
    };
  }

  @Override
  public LongConsumer addIntegerPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initInteger() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setInteger(value);
      }
    };
  }

  @Override
  public FloatConsumer addFloatPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initFloat() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setFloat(value);
      }
    };
  }

  @Override
  public DoubleConsumer addDoublePublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initDouble() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setDouble(value);
      }
    };
  }

  @Override
  public Consumer<String> addStringPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initString() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setString(value);
      }
    };
  }

  @Override
  public Consumer<boolean[]> addBooleanArrayPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initBooleanArray() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setBooleanArray(value);
      }
    };
  }

  @Override
  public Consumer<long[]> addIntegerArrayPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initIntegerArray() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setIntegerArray(value);
      }
    };
  }

  @Override
  public Consumer<float[]> addFloatArrayPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initFloatArray() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setFloatArray(value);
      }
    };
  }

  @Override
  public Consumer<double[]> addDoubleArrayPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initDoubleArray() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setDoubleArray(value);
      }
    };
  }

  @Override
  public Consumer<String[]> addStringArrayPublisher(String name) {
    EntryData data = getOrNew(name);
    if (data.initStringArray() == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setStringArray(value);
      }
    };
  }

  @Override
  public Consumer<byte[]> addRawBytesPublisher(String name, String typeString) {
    EntryData data = getOrNew(name);
    if (data.initRaw(typeString) == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setRaw(value);
      }
    };
  }

  @Override
  public Consumer<ByteBuffer> addRawBufferPublisher(String name, String typeString) {
    EntryData data = getOrNew(name);
    if (data.initRaw(typeString) == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        d.setRaw(value);
      }
    };
  }

  @Override
  public <T> Consumer<T> addStructPublisher(String name, Struct<T> struct) {
    EntryData data = getOrNew(name);
    if (data.initStruct(struct) == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        ByteBuffer bb = buf.write(value);
        data.setRaw(bb, 0, bb.position());
      }
    };
  }

  @Override
  public <T> Consumer<T> addProtobufPublisher(String name, Protobuf<T, ?> proto) {
    EntryData data = getOrNew(name);
    if (data.initProtobuf(proto) == null) {
      return value -> {};
    }
    final WeakReference<EntryData> dataRef = new WeakReference<>(data);
    final ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    return value -> {
      EntryData d = dataRef.get();
      if (d != null && d.m_entry != null) {
        try {
          ByteBuffer bb = buf.write(value);
          data.setRaw(bb, 0, bb.position());
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
    EntryData data = getOrNew(name);
    for (SendableOption option : options) {
      if (option.getKind() == SendableOption.Kind.kKeepDuplicates) {
        data.m_appendAll = option.getBooleanValue();
      }
    }
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
    String value;
    synchronized (data) {
      value = data.m_propertiesMap.get(propName);
    }
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
    EntryData data = m_entries.computeIfAbsent(name, k -> new EntryData(m_log, name));
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
    EntryData data = m_entries.computeIfAbsent(name, k -> new EntryData(m_log, name));
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
    DataLogSendableTable table = m_tables.remove(name);
    if (table != null) {
      table.close();
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
      if (consumer != null && data.m_entry != null) {
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
