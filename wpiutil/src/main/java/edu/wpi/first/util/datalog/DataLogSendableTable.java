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
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
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

  private static enum DataType {
    kNone, kBoolean, kInteger, kFloat, kDouble, kString, kBooleanArray, kIntegerArray, kFloatArray, kDoubleArray, kStringArray, kRaw, kStruct, kProtobuf;
  }

  private static class EntryData {
    DataLog log;
    int entry;
    DataType type = DataType.kNone;

    // change tracking
    boolean hasValue;
    boolean booleanValue;
    long intValue;
    float floatValue;
    double doubleValue;
    String stringValue;
    boolean[] booleanArrayValue;
    long[] intArrayValue;
    float[] floatArrayValue;
    double[] doubleArrayValue;
    String[] stringArrayValue;
    byte[] rawValue;
    ByteBuffer rawByteBufferValue;

    String properties = new String();
    StructBuffer<?> structBuffer;
    ProtobufBuffer<?, ?> protobufBuffer;
    Consumer<EntryData> polledUpdate;

    boolean appendAll;

    void close() {
      if (entry != 0) {
        log.finish(entry);
        entry = 0;
      }
    }

    void setBoolean(boolean value) {
      if (type != DataType.kBoolean) {
        return;
      }
      if (!appendAll && hasValue && booleanValue == value) {
        return;
      }
      hasValue = true;
      booleanValue = value;
      log.appendBoolean(entry, value, 0);
    }

    void setInteger(long value) {
      if (type != DataType.kInteger) {
        return;
      }
      if (!appendAll && hasValue && intValue == value) {
        return;
      }
      hasValue = true;
      intValue = value;
      log.appendInteger(entry, value, 0);
    }

    void setFloat(float value) {
      if (type != DataType.kFloat) {
        return;
      }
      if (!appendAll && hasValue && floatValue == value) {
        return;
      }
      hasValue = true;
      floatValue = value;
      log.appendFloat(entry, value, 0);
    }

    void setDouble(double value) {
      if (type != DataType.kDouble) {
        return;
      }
      if (!appendAll && hasValue && doubleValue == value) {
        return;
      }
      hasValue = true;
      doubleValue = value;
      log.appendDouble(entry, value, 0);
    }

    void setString(String value) {
      if (type != DataType.kString) {
        return;
      }
      if (!appendAll && hasValue && stringValue.equals(value)) {
        return;
      }
      hasValue = true;
      stringValue = value;
      log.appendString(entry, value, 0);
    }

    void setBooleanArray(boolean[] value) {
      if (type != DataType.kBooleanArray) {
        return;
      }
      if (!appendAll && hasValue && Arrays.equals(booleanArrayValue, value)) {
        return;
      }
      hasValue = true;
      booleanArrayValue = Arrays.copyOf(value, value.length);
      log.appendBooleanArray(entry, value, 0);
    }

    void setIntegerArray(long[] value) {
      if (type != DataType.kIntegerArray) {
        return;
      }
      if (!appendAll && hasValue && Arrays.equals(intArrayValue, value)) {
        return;
      }
      hasValue = true;
      intArrayValue = Arrays.copyOf(value, value.length);
      log.appendIntegerArray(entry, value, 0);
    }

    void setFloatArray(float[] value) {
      if (type != DataType.kFloatArray) {
        return;
      }
      if (!appendAll && hasValue && Arrays.equals(floatArrayValue, value)) {
        return;
      }
      hasValue = true;
      floatArrayValue = Arrays.copyOf(value, value.length);
      log.appendFloatArray(entry, value, 0);
    }

    void setDoubleArray(double[] value) {
      if (type != DataType.kDoubleArray) {
        return;
      }
      if (!appendAll && hasValue && Arrays.equals(doubleArrayValue, value)) {
        return;
      }
      hasValue = true;
      doubleArrayValue = Arrays.copyOf(value, value.length);
      log.appendDoubleArray(entry, value, 0);
    }

    void setStringArray(String[] value) {
      if (type != DataType.kStringArray) {
        return;
      }
      if (!appendAll && hasValue && Arrays.equals(stringArrayValue, value)) {
        return;
      }
      hasValue = true;
      stringArrayValue = Arrays.copyOf(value, value.length);
      log.appendStringArray(entry, value, 0);
    }

    boolean rawEquals(byte[] original, byte[] value, int start, int len) {
      if (len != original.length) {
        return false;
      }
      for (int i = start, j = 0; j < len; ++i, ++j) {
        if (value[i] != original[j]) {
          return false;
        }
      }
      return true;
    }

    void setRaw(byte[] value) {
      setRaw(value, 0, value.length);
    }

    void setRaw(byte[] value, int start, int len) {
      if (type != DataType.kRaw && type != DataType.kStruct && type != DataType.kProtobuf) {
        return;
      }
      if (!appendAll && hasValue && rawEquals(rawValue, value, start, len)) {
        return;
      }
      hasValue = true;
      rawValue = new byte[len];
      System.arraycopy(value, start, rawValue, 0, len);
      log.appendRaw(entry, value, start, len, 0);
    }

    void setRaw(ByteBuffer value) {
      int pos = value.position();
      setRaw(value, pos, value.limit() - pos);
    }

    void setRaw(ByteBuffer value, int start, int len) {
      if (type != DataType.kRaw && type != DataType.kStruct && type != DataType.kProtobuf) {
        return;
      }
      if (!appendAll && hasValue && rawEquals(rawValue, value, start, len)) {
        return;
      }
      hasValue = true;
      rawValue = new byte[len];
      if (value.hasArray()) {
        System.arraycopy(value.array(), value.arrayOffset() + start, rawValue, 0, len);
      } else if (value.isDirect()) {
        int curPos = value.position();
        value.get(rawValue, 0, len);
        value.position(curPos);
      }
      log.appendRaw(entry, value, start, len, 0);
    }
  }
  private final Map<String, EntryData> m_entries = new HashMap<>();
  private final Map<String, DataLogSendableTable> m_tables = new HashMap<>();
  private Consumer<SendableTable> m_closeSendable;
  private boolean m_closed;

  private EntryData start(String name, String typeString, DataType type) {
    EntryData data = m_entries.computeIfAbsent(name, k -> new EntryData());
    if (data.entry == 0) {
      data.log = m_log;
      data.entry = m_log.start(m_pathWithSep + name, typeString, data.properties, 0);
      data.type = type;
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
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kBoolean || !data.hasValue) {
      return defaultValue;
    }
    return data.booleanValue;
  }

  @Override
  public long getInteger(String name, long defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kInteger || !data.hasValue) {
      return defaultValue;
    }
    return data.intValue;
  }

  @Override
  public float getFloat(String name, float defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kFloat || !data.hasValue) {
      return defaultValue;
    }
    return data.floatValue;
  }

  @Override
  public double getDouble(String name, double defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kDouble || !data.hasValue) {
      return defaultValue;
    }
    return data.doubleValue;
  }

  @Override
  public String getString(String name, String defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kString || !data.hasValue) {
      return defaultValue;
    }
    return data.stringValue;
  }

  @Override
  public boolean[] getBooleanArray(String name, boolean[] defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kBooleanArray || !data.hasValue) {
      return defaultValue;
    }
    return data.booleanArrayValue;
  }

  @Override
  public long[] getIntegerArray(String name, long[] defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kIntegerArray || !data.hasValue) {
      return defaultValue;
    }
    return data.intArrayValue;
  }

  @Override
  public float[] getFloatArray(String name, float[] defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kFloatArray || !data.hasValue) {
      return defaultValue;
    }
    return data.floatArrayValue;
  }

  @Override
  public double[] getDoubleArray(String name, double[] defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kDoubleArray || !data.hasValue) {
      return defaultValue;
    }
    return data.doubleArrayValue;
  }

  @Override
  public String[] getStringArray(String name, String[] defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kStringArray || !data.hasValue) {
      return defaultValue;
    }
    return data.stringArrayValue;
  }

  @Override
  public byte[] getRaw(String name, String typeString, byte[] defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kRaw || !data.hasValue) {
      return defaultValue;
    }
    return data.rawValue;
  }

  @Override
  public <T> T getStruct(String name, Struct<T> struct, T defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kStruct || !data.hasValue || data.structBuffer.getStruct() != struct) {
      return defaultValue;
    }
    @SuppressWarnings("unchecked")
    StructBuffer<T> buf = ((StructBuffer<T>) data.structBuffer);
    return buf.read(data.rawValue);
  }

  @Override
  public <T> boolean getStructInto(String name, T out, Struct<T> struct) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kStruct || !data.hasValue || data.structBuffer.getStruct() != struct) {
      return false;
    }
    @SuppressWarnings("unchecked")
    StructBuffer<T> buf = ((StructBuffer<T>) data.structBuffer);
    buf.readInto(out, data.rawValue);
    return true;
  }

  @Override
  public <T> T getProtobuf(String name, Protobuf<T, ?> proto, T defaultValue) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kProtobuf || !data.hasValue || data.protobufBuffer.getProto() != proto) {
      return defaultValue;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) data.protobufBuffer);
    try {
      return buf.read(data.rawValue);
    } catch (IOException e) {
      return defaultValue;
    }
  }

  @Override
  public <T> boolean getProtobufInto(String name, T out, Protobuf<T, ?> proto) {
    EntryData data = m_entries.get(name);
    if (data == null || data.type != DataType.kProtobuf || !data.hasValue || data.protobufBuffer.getProto() != proto) {
      return false;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) data.protobufBuffer);
    try {
      buf.readInto(out, data.rawValue);
    } catch (IOException e) {
      return false;
    }
    return true;
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
    if (td.structBuffer == null) {
      td.structBuffer = StructBuffer.create(struct);
      addSchema(struct);
    } else if (td.structBuffer.getStruct() != struct) {
      return;
    }
    @SuppressWarnings("unchecked")
    StructBuffer<T> buf = ((StructBuffer<T>) td.structBuffer);
    ByteBuffer bb = buf.write(value);
    td.setRaw(bb, 0, bb.position());
  }

  @Override
  public <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto) {
    EntryData td = start(name, proto.getTypeString(), DataType.kProtobuf);
    if (td.protobufBuffer == null) {
      td.protobufBuffer = ProtobufBuffer.create(proto);
      addSchema(proto);
    } else if (td.protobufBuffer.getProto() != proto) {
      return;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.protobufBuffer);
    try {
      ByteBuffer bb = buf.write(value);
      td.setRaw(bb, 0, bb.position());
    } catch (IOException e) {
      // ignore
    }
  }

  @Override
  public void publishBoolean(String name, BooleanSupplier supplier) {
    EntryData data = start(name, "boolean", DataType.kBoolean);
    data.polledUpdate = entry -> { entry.setBoolean(supplier.getAsBoolean()); };
  }

  @Override
  public void publishInteger(String name, LongSupplier supplier) {
    EntryData data = start(name, "int64", DataType.kInteger);
    data.polledUpdate = entry -> { entry.setInteger(supplier.getAsLong()); };
  }

  @Override
  public void publishFloat(String name, FloatSupplier supplier) {
    EntryData data = start(name, "float", DataType.kFloat);
    data.polledUpdate = entry -> { entry.setFloat(supplier.getAsFloat()); };
  }

  @Override
  public void publishDouble(String name, DoubleSupplier supplier) {
    EntryData data = start(name, "double", DataType.kDouble);
    data.polledUpdate = entry -> { entry.setDouble(supplier.getAsDouble()); };
  }

  @Override
  public void publishString(String name, Supplier<String> supplier) {
    EntryData data = start(name, "string", DataType.kString);
    data.polledUpdate = entry -> { entry.setString(supplier.get()); };
  }

  @Override
  public void publishBooleanArray(String name, Supplier<boolean[]> supplier) {
    EntryData data = start(name, "boolean[]", DataType.kBooleanArray);
    data.polledUpdate = entry -> { entry.setBooleanArray(supplier.get()); };
  }

  @Override
  public void publishIntegerArray(String name, Supplier<long[]> supplier) {
    EntryData data = start(name, "int64[]", DataType.kStringArray);
    data.polledUpdate = entry -> { entry.setIntegerArray(supplier.get()); };
  }

  @Override
  public void publishFloatArray(String name, Supplier<float[]> supplier) {
    EntryData data = start(name, "float[]", DataType.kFloatArray);
    data.polledUpdate = entry -> { entry.setFloatArray(supplier.get()); };
  }

  @Override
  public void publishDoubleArray(String name, Supplier<double[]> supplier) {
    EntryData data = start(name, "double[]", DataType.kDoubleArray);
    data.polledUpdate = entry -> { entry.setDoubleArray(supplier.get()); };
  }

  @Override
  public void publishStringArray(String name, Supplier<String[]> supplier) {
    EntryData data = start(name, "string[]", DataType.kStringArray);
    data.polledUpdate = entry -> { entry.setStringArray(supplier.get()); };
  }

  @Override
  public void publishRawBytes(String name, String typeString, Supplier<byte[]> supplier) {
    EntryData data = start(name, typeString, DataType.kRaw);
    data.polledUpdate = entry -> { entry.setRaw(supplier.get()); };
  }

  @Override
  public void publishRawBuffer(String name, String typeString, Supplier<ByteBuffer> supplier) {
    EntryData data = start(name, typeString, DataType.kRaw);
    data.polledUpdate = entry -> { entry.setRaw(supplier.get()); };
  }

  @Override
  public <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    EntryData td = start(name, struct.getTypeString(), DataType.kStruct);
    if (td.structBuffer == null) {
      td.structBuffer = StructBuffer.create(struct);
      addSchema(struct);
    } else if (td.structBuffer.getStruct() != struct) {
      return;
    }
    @SuppressWarnings("unchecked")
    final StructBuffer<T> buf = ((StructBuffer<T>) td.structBuffer);
    td.polledUpdate = entry -> { entry.setRaw(buf.write(supplier.get())); };
  }

  @Override
  public <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier) {
    EntryData td = start(name, proto.getTypeString(), DataType.kProtobuf);
    if (td.protobufBuffer == null) {
      td.protobufBuffer = ProtobufBuffer.create(proto);
      addSchema(proto);
    } else if (td.protobufBuffer.getProto() != proto) {
      return;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.protobufBuffer);
    td.polledUpdate = entry -> {
      try {
        entry.setRaw(buf.write(supplier.get()));
      } catch (IOException e) {
        return; // ignore
      }
    };
  }

  @Override
  public BooleanConsumer addBooleanPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "boolean", DataType.kBoolean));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setBoolean(value);
      }
    };
  }

  @Override
  public LongConsumer addIntegerPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "int64", DataType.kInteger));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setInteger(value);
      }
    };
  }

  @Override
  public FloatConsumer addFloatPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "float", DataType.kFloat));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setFloat(value);
      }
    };
  }

  @Override
  public DoubleConsumer addDoublePublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "double", DataType.kDouble));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setDouble(value);
      }
    };
  }

  @Override
  public Consumer<String> addStringPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "string", DataType.kString));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setString(value);
      }
    };
  }

  @Override
  public Consumer<boolean[]> addBooleanArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "boolean[]", DataType.kBooleanArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setBooleanArray(value);
      }
    };
  }

  @Override
  public Consumer<long[]> addIntegerArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "int64[]", DataType.kIntegerArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setIntegerArray(value);
      }
    };
  }

  @Override
  public Consumer<float[]> addFloatArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "float[]", DataType.kFloatArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setFloatArray(value);
      }
    };
  }

  @Override
  public Consumer<double[]> addDoubleArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "double[]", DataType.kDoubleArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setDoubleArray(value);
      }
    };
  }

  @Override
  public Consumer<String[]> addStringArrayPublisher(String name) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, "string[]", DataType.kStringArray));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setStringArray(value);
      }
    };
  }

  @Override
  public Consumer<byte[]> addRawBytesPublisher(String name, String typeString) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, typeString, DataType.kRaw));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setRaw(value);
      }
    };
  }

  @Override
  public Consumer<ByteBuffer> addRawBufferPublisher(String name, String typeString) {
    final WeakReference<EntryData> dataRef = new WeakReference<>(start(name, typeString, DataType.kRaw));
    return value -> {
      EntryData data = dataRef.get();
      if (data != null && data.entry != 0) {
        data.setRaw(value);
      }
    };
  }

  @Override
  public void subscribeBoolean(String name, BooleanConsumer consumer) {
  }

  @Override
  public void subscribeInteger(String name, LongConsumer consumer) {
  }

  @Override
  public void subscribeFloat(String name, FloatConsumer consumer) {
  }

  @Override
  public void subscribeDouble(String name, DoubleConsumer consumer) {
  }

  @Override
  public void subscribeString(String name, Consumer<String> consumer) {
  }

  @Override
  public void subscribeBooleanArray(String name, Consumer<boolean[]> consumer) {
  }

  @Override
  public void subscribeIntegerArray(String name, Consumer<long[]> consumer) {
  }

  @Override
  public void subscribeFloatArray(String name, Consumer<float[]> consumer) {
  }

  @Override
  public void subscribeDoubleArray(String name, Consumer<double[]> consumer) {
  }

  @Override
  public void subscribeStringArray(String name, Consumer<String[]> consumer) {
  }

  @Override
  public void subscribeRawBytes(String name, String typeString, Consumer<byte[]> consumer) {
  }

  @Override
  public <T> DataLogSendableTable addSendable(String name, T obj, Sendable<T> sendable) {
    DataLogSendableTable child = getChild(name);
    if (child.m_closeSendable == null) {
      sendable.initSendable(obj, child);
      child.m_closeSendable = table -> { sendable.closeSendable(obj, table); };
    }
    return child;
  }

  @SuppressWarnings("resource")
  @Override
  public DataLogSendableTable getChild(String name) {
    return m_tables.computeIfAbsent(name, k -> new DataLogSendableTable(m_log, m_pathWithSep + name));
  }

  @Override
  public void setPublishOptions(String name, SendableOption... options) {
    // TODO
  }

  @Override
  public void setSubscribeOptions(String name, SendableOption... options) {
    // TODO
  }

  /**
   * Gets the current value of a property (as a JSON string).
   *
   * @param name name
   * @param propName property name
   * @return JSON string; "null" if the property does not exist.
   */
  @Override
  public String getProperty(String name, String propName) {
    return "null"; // TODO
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
    // TODO
  }

  /**
   * Deletes a property. Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  @Override
  public void deleteProperty(String name, String propName) {
    // TODO
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
    return "{}"; // TODO
  }

  /**
   * Updates multiple topic properties. Each key in the passed-in object is the name of the property
   * to add/update, and the corresponding value is the property value to set for that property. Null
   * values result in deletion of the corresponding property.
   *
   * @param name name
   * @param properties JSON object string with keys to add/update/delete
   * @throws IllegalArgumentException if properties is not a JSON object
   */
  @Override
  public void setProperties(String name, String properties) {
    // TODO
  }

  @Override
  public void remove(String name) {
    EntryData data = m_entries.remove(name);
    if (data != null) {
      data.close();
    }
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
      if (data.polledUpdate != null && data.entry != 0) {
        data.polledUpdate.accept(data);
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
   * instance has published. This does NOT perform a check as to whether the schema has already
   * been published by another node on the network.
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
