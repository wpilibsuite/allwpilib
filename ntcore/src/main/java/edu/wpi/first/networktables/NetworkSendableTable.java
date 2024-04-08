// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

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
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** NetworkTables-backed implementation of SendableTable. */
public class NetworkSendableTable implements SendableTable {
  private static final char PATH_SEPARATOR = '/';

  private final String m_path;
  private final String m_pathWithSep;
  private final NetworkTableInstance m_inst;

  private static class TopicData {
    void close() {
      if (publisher != null) {
        publisher.close();
      }
      if (subscriber != null) {
        subscriber.close();
      }
    }

    Topic topic;
    GenericPublisher publisher;
    GenericSubscriber subscriber;
    PubSubOption[] options = new PubSubOption[0];
    StructBuffer<?> structBuffer;
    ProtobufBuffer<?, ?> protobufBuffer;
    Consumer<GenericPublisher> polledUpdate;
  }
  private final Map<String, TopicData> m_topics = new HashMap<>();
  private final Map<String, NetworkSendableTable> m_tables = new HashMap<>();
  private Consumer<SendableTable> m_closeSendable;
  private boolean m_closed;

  private TopicData getTopicData(String name) {
    TopicData data = m_topics.computeIfAbsent(name, k -> new TopicData());
    if (data.topic == null) {
      data.topic = m_inst.getTopic(m_pathWithSep + name);
    }
    return data;
  }

  private TopicData subscribe(String name, String typeString) {
    TopicData data = getTopicData(name);
    if (data.subscriber == null) {
      data.subscriber = data.topic.genericSubscribe(typeString, data.options);
    }
    return data;
  }

  private TopicData publish(String name, String typeString) {
    TopicData data = getTopicData(name);
    if (data.publisher == null) {
      data.publisher = data.topic.genericPublish(typeString, data.options);
    }
    return data;
  }

  public NetworkSendableTable(NetworkTableInstance inst, String path) {
    m_path = path;
    m_pathWithSep = path + PATH_SEPARATOR;
    m_inst = inst;
  }

  /**
   * Gets the instance for the table.
   *
   * @return Instance
   */
  public NetworkTableInstance getInstance() {
    return m_inst;
  }

  @Override
  public String toString() {
    return "NetworkSendableTable: " + m_path;
  }

  @Override
  public boolean getBoolean(String name, boolean defaultValue) {
    return subscribe(name, "boolean").subscriber.getBoolean(defaultValue);
  }

  @Override
  public long getInteger(String name, long defaultValue) {
    return subscribe(name, "int").subscriber.getInteger(defaultValue);
  }

  @Override
  public float getFloat(String name, float defaultValue) {
    return subscribe(name, "float").subscriber.getFloat(defaultValue);
  }

  @Override
  public double getDouble(String name, double defaultValue) {
    return subscribe(name, "double").subscriber.getDouble(defaultValue);
  }

  @Override
  public String getString(String name, String defaultValue) {
    return subscribe(name, "string").subscriber.getString(defaultValue);
  }

  @Override
  public boolean[] getBooleanArray(String name, boolean[] defaultValue) {
    return subscribe(name, "boolean[]").subscriber.getBooleanArray(defaultValue);
  }

  @Override
  public long[] getIntegerArray(String name, long[] defaultValue) {
    return subscribe(name, "int[]").subscriber.getIntegerArray(defaultValue);
  }

  @Override
  public float[] getFloatArray(String name, float[] defaultValue) {
    return subscribe(name, "float[]").subscriber.getFloatArray(defaultValue);
  }

  @Override
  public double[] getDoubleArray(String name, double[] defaultValue) {
    return subscribe(name, "double[]").subscriber.getDoubleArray(defaultValue);
  }

  @Override
  public String[] getStringArray(String name, String[] defaultValue) {
    return subscribe(name, "string[]").subscriber.getStringArray(defaultValue);
  }

  @Override
  public byte[] getRaw(String name, String typeString, byte[] defaultValue) {
    return subscribe(name, typeString).subscriber.getRaw(defaultValue);
  }

  @Override
  public <T> T getStruct(String name, Struct<T> struct, T defaultValue) {
    TopicData td = getTopicData(name);
    if (td.subscriber == null) {
      td.subscriber = td.topic.genericSubscribe(struct.getTypeString(), td.options);
    }
    byte[] data = td.subscriber.getRaw(null);
    if (data == null) {
      return defaultValue;
    }
    if (td.structBuffer == null) {
      td.structBuffer = StructBuffer.create(struct);
    } else if (td.structBuffer.getStruct() != struct) {
      return defaultValue;
    }
    @SuppressWarnings("unchecked")
    StructBuffer<T> buf = ((StructBuffer<T>) td.structBuffer);
    return buf.read(data);
  }

  @Override
  public <T> boolean getStructInto(String name, T out, Struct<T> struct) {
    TopicData td = getTopicData(name);
    if (td.subscriber == null) {
      td.subscriber = td.topic.genericSubscribe(struct.getTypeString(), td.options);
    }
    byte[] data = td.subscriber.getRaw(null);
    if (data == null) {
      return false;
    }
    if (td.structBuffer == null) {
      td.structBuffer = StructBuffer.create(struct);
    } else if (td.structBuffer.getStruct() != struct) {
      return false;
    }
    @SuppressWarnings("unchecked")
    StructBuffer<T> buf = ((StructBuffer<T>) td.structBuffer);
    buf.readInto(out, data);
    return true;
  }

  @Override
  public <T> T getProtobuf(String name, Protobuf<T, ?> proto, T defaultValue) {
    TopicData td = getTopicData(name);
    if (td.subscriber == null) {
      td.subscriber = td.topic.genericSubscribe(proto.getTypeString(), td.options);
    }
    byte[] data = td.subscriber.getRaw(null);
    if (data == null) {
      return defaultValue;
    }
    if (td.protobufBuffer == null) {
      td.protobufBuffer = ProtobufBuffer.create(proto);
    } else if (td.protobufBuffer.getProto() != proto) {
      return defaultValue;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.protobufBuffer);
    try {
      return buf.read(data);
    } catch (IOException e) {
      return defaultValue;
    }
  }

  @Override
  public <T> boolean getProtobufInto(String name, T out, Protobuf<T, ?> proto) {
    TopicData td = getTopicData(name);
    if (td.subscriber == null) {
      td.subscriber = td.topic.genericSubscribe(proto.getTypeString(), td.options);
    }
    byte[] data = td.subscriber.getRaw(null);
    if (data == null) {
      return false;
    }
    if (td.protobufBuffer == null) {
      td.protobufBuffer = ProtobufBuffer.create(proto);
    } else if (td.protobufBuffer.getProto() != proto) {
      return false;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.protobufBuffer);
    try {
      buf.readInto(out, data);
    } catch (IOException e) {
      return false;
    }
    return true;
  }

  @Override
  public void setBoolean(String name, boolean value) {
    publish(name, "boolean").publisher.setBoolean(value);
  }

  @Override
  public void setInteger(String name, long value) {
    publish(name, "int").publisher.setInteger(value);
  }

  @Override
  public void setFloat(String name, float value) {
    publish(name, "float").publisher.setFloat(value);
  }

  @Override
  public void setDouble(String name, double value) {
    publish(name, "double").publisher.setDouble(value);
  }

  @Override
  public void setString(String name, String value) {
    publish(name, "string").publisher.setString(value);
  }

  @Override
  public void setBooleanArray(String name, boolean[] value) {
    publish(name, "boolean[]").publisher.setBooleanArray(value);
  }

  @Override
  public void setIntegerArray(String name, long[] value) {
    publish(name, "int[]").publisher.setIntegerArray(value);
  }

  @Override
  public void setFloatArray(String name, float[] value) {
    publish(name, "float[]").publisher.setFloatArray(value);
  }

  @Override
  public void setDoubleArray(String name, double[] value) {
    publish(name, "double[]").publisher.setDoubleArray(value);
  }

  @Override
  public void setStringArray(String name, String[] value) {
    publish(name, "string[]").publisher.setStringArray(value);
  }

  @Override
  public void setRaw(String name, String typeString, byte[] value, int start, int len) {
    publish(name, typeString).publisher.setRaw(value, start, len);
  }

  @Override
  public void setRaw(String name, String typeString, ByteBuffer value, int start, int len) {
    publish(name, typeString).publisher.setRaw(value, start, len);
  }

  @Override
  public <T> void setStruct(String name, T value, Struct<T> struct) {
    TopicData td = getTopicData(name);
    if (td.publisher == null) {
      td.publisher = td.topic.genericPublish(struct.getTypeString(), td.options);
      addSchema(struct);
    }
    if (td.structBuffer == null) {
      td.structBuffer = StructBuffer.create(struct);
    } else if (td.structBuffer.getStruct() != struct) {
      return;
    }
    @SuppressWarnings("unchecked")
    StructBuffer<T> buf = ((StructBuffer<T>) td.structBuffer);
    ByteBuffer bb = buf.write(value);
    td.publisher.setRaw(bb, 0, bb.position());
  }

  @Override
  public <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto) {
    TopicData td = getTopicData(name);
    if (td.publisher == null) {
      td.publisher = td.topic.genericPublish(proto.getTypeString(), td.options);
      addSchema(proto);
    }
    if (td.protobufBuffer == null) {
      td.protobufBuffer = ProtobufBuffer.create(proto);
    } else if (td.protobufBuffer.getProto() != proto) {
      return;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.protobufBuffer);
    try {
      ByteBuffer bb = buf.write(value);
      td.publisher.setRaw(bb, 0, bb.position());
    } catch (IOException e) {
      // ignore
    }
  }

  @Override
  public void publishBoolean(String name, BooleanSupplier supplier) {
    TopicData data = publish(name, "boolean");
    data.polledUpdate = pub -> { pub.setBoolean(supplier.getAsBoolean()); };
  }

  @Override
  public void publishInteger(String name, LongSupplier supplier) {
    TopicData data = publish(name, "int");
    data.polledUpdate = pub -> { pub.setInteger(supplier.getAsLong()); };
  }

  @Override
  public void publishFloat(String name, FloatSupplier supplier) {
    TopicData data = publish(name, "float");
    data.polledUpdate = pub -> { pub.setFloat(supplier.getAsFloat()); };
  }

  @Override
  public void publishDouble(String name, DoubleSupplier supplier) {
    TopicData data = publish(name, "double");
    data.polledUpdate = pub -> { pub.setDouble(supplier.getAsDouble()); };
  }

  @Override
  public void publishString(String name, Supplier<String> supplier) {
    TopicData data = publish(name, "string");
    data.polledUpdate = pub -> { pub.setString(supplier.get()); };
  }

  @Override
  public void publishBooleanArray(String name, Supplier<boolean[]> supplier) {
    TopicData data = publish(name, "boolean[]");
    data.polledUpdate = pub -> { pub.setBooleanArray(supplier.get()); };
  }

  @Override
  public void publishIntegerArray(String name, Supplier<long[]> supplier) {
    TopicData data = publish(name, "int[]");
    data.polledUpdate = pub -> { pub.setIntegerArray(supplier.get()); };
  }

  @Override
  public void publishFloatArray(String name, Supplier<float[]> supplier) {
    TopicData data = publish(name, "float[]");
    data.polledUpdate = pub -> { pub.setFloatArray(supplier.get()); };
  }

  @Override
  public void publishDoubleArray(String name, Supplier<double[]> supplier) {
    TopicData data = publish(name, "double[]");
    data.polledUpdate = pub -> { pub.setDoubleArray(supplier.get()); };
  }

  @Override
  public void publishStringArray(String name, Supplier<String[]> supplier) {
    TopicData data = publish(name, "string[]");
    data.polledUpdate = pub -> { pub.setStringArray(supplier.get()); };
  }

  @Override
  public void publishRawBytes(String name, String typeString, Supplier<byte[]> supplier) {
    TopicData data = publish(name, typeString);
    data.polledUpdate = pub -> { pub.setRaw(supplier.get()); };
  }

  @Override
  public void publishRawBuffer(String name, String typeString, Supplier<ByteBuffer> supplier) {
    TopicData data = publish(name, typeString);
    data.polledUpdate = pub -> { pub.setRaw(supplier.get()); };
  }

  @Override
  public <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    TopicData td = getTopicData(name);
    if (td.publisher == null) {
      td.publisher = td.topic.genericPublish(struct.getTypeString(), td.options);
      addSchema(struct);
    }
    if (td.structBuffer == null) {
      td.structBuffer = StructBuffer.create(struct);
    } else if (td.structBuffer.getStruct() != struct) {
      return;
    }
    @SuppressWarnings("unchecked")
    final StructBuffer<T> buf = ((StructBuffer<T>) td.structBuffer);
    td.polledUpdate = pub -> { pub.setRaw(buf.write(supplier.get())); };
  }

  @Override
  public <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier) {
    TopicData td = getTopicData(name);
    if (td.publisher == null) {
      td.publisher = td.topic.genericPublish(proto.getTypeString(), td.options);
      addSchema(proto);
    }
    if (td.protobufBuffer == null) {
      td.protobufBuffer = ProtobufBuffer.create(proto);
    } else if (td.protobufBuffer.getProto() != proto) {
      return;
    }
    @SuppressWarnings("unchecked")
    final ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.protobufBuffer);
    td.polledUpdate = pub -> {
      try {
        pub.setRaw(buf.write(supplier.get()));
      } catch (IOException e) {
        return; // ignore
      }
    };
  }

  @Override
  public BooleanConsumer addBooleanPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "boolean"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setBoolean(value);
      }
    };
  }

  @Override
  public LongConsumer addIntegerPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "int"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setInteger(value);
      }
    };
  }

  @Override
  public FloatConsumer addFloatPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "float"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setFloat(value);
      }
    };
  }

  @Override
  public DoubleConsumer addDoublePublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "double"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setDouble(value);
      }
    };
  }

  @Override
  public Consumer<String> addStringPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "string"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setString(value);
      }
    };
  }

  @Override
  public Consumer<boolean[]> addBooleanArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "boolean[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setBooleanArray(value);
      }
    };
  }

  @Override
  public Consumer<long[]> addIntegerArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "int[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setIntegerArray(value);
      }
    };
  }

  @Override
  public Consumer<float[]> addFloatArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "float[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setFloatArray(value);
      }
    };
  }

  @Override
  public Consumer<double[]> addDoubleArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "double[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setDoubleArray(value);
      }
    };
  }

  @Override
  public Consumer<String[]> addStringArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "string[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setStringArray(value);
      }
    };
  }

  @Override
  public Consumer<byte[]> addRawBytesPublisher(String name, String typeString) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, typeString));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setRaw(value);
      }
    };
  }

  @Override
  public Consumer<ByteBuffer> addRawBufferPublisher(String name, String typeString) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, typeString));
    return value -> {
      TopicData data = dataRef.get();
      if (data != null && data.publisher != null && data.publisher.isValid()) {
        data.publisher.setRaw(value);
      }
    };
  }

  @Override
  public void subscribeBoolean(String name, BooleanConsumer consumer) {
    // TODO
  }

  @Override
  public void subscribeInteger(String name, LongConsumer consumer) {
    // TODO
  }

  @Override
  public void subscribeFloat(String name, FloatConsumer consumer) {
    // TODO
  }

  @Override
  public void subscribeDouble(String name, DoubleConsumer consumer) {
    // TODO
  }

  @Override
  public void subscribeString(String name, Consumer<String> consumer) {
    // TODO
  }

  @Override
  public void subscribeBooleanArray(String name, Consumer<boolean[]> consumer) {
    // TODO
  }

  @Override
  public void subscribeIntegerArray(String name, Consumer<long[]> consumer) {
    // TODO
  }

  @Override
  public void subscribeFloatArray(String name, Consumer<float[]> consumer) {
    // TODO
  }

  @Override
  public void subscribeDoubleArray(String name, Consumer<double[]> consumer) {
    // TODO
  }

  @Override
  public void subscribeStringArray(String name, Consumer<String[]> consumer) {
    // TODO
  }

  @Override
  public void subscribeRawBytes(String name, String typeString, Consumer<byte[]> consumer) {
    // TODO
  }

  @Override
  public <T> NetworkSendableTable addSendable(String name, T obj, Sendable<T> sendable) {
    NetworkSendableTable child = getChild(name);
    if (child.m_closeSendable == null) {
      sendable.initSendable(obj, child);
      child.m_closeSendable = table -> { sendable.closeSendable(obj, table); };
    }
    return child;
  }

  @SuppressWarnings("resource")
  @Override
  public NetworkSendableTable getChild(String name) {
    return m_tables.computeIfAbsent(name, k -> new NetworkSendableTable(m_inst, m_pathWithSep + name));
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
    return getTopicData(name).topic.getProperty(propName);
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
    getTopicData(name).topic.setProperty(propName, value);
  }

  /**
   * Deletes a property. Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  @Override
  public void deleteProperty(String name, String propName) {
    getTopicData(name).topic.deleteProperty(propName);
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
    return getTopicData(name).topic.getProperties();
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
    getTopicData(name).topic.setProperties(properties);
  }

  @Override
  public void remove(String name) {
    TopicData data = m_topics.remove(name);
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
    for (TopicData data : m_topics.values()) {
      if (data.polledUpdate != null && data.publisher != null) {
        data.polledUpdate.accept(data.publisher);
      }
    }
    for (NetworkSendableTable table : m_tables.values()) {
      table.update();
    }
  }

  /** Erases all publishers and subscribers. */
  @Override
  public void clear() {
    for (TopicData data : m_topics.values()) {
      data.close();
    }
    m_topics.clear();
    for (NetworkSendableTable table : m_tables.values()) {
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
    return m_inst.hasSchema(name);
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
    m_inst.addSchema(name, type, schema);
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
    m_inst.addSchema(name, type, schema);
  }

  /**
   * Registers a protobuf schema. Duplicate calls to this function with the same name are silently
   * ignored.
   *
   * @param proto protobuf serialization object
   */
  @Override
  public void addSchema(Protobuf<?, ?> proto) {
    m_inst.addSchema(proto);
  }

  /**
   * Registers a struct schema. Duplicate calls to this function with the same name are silently
   * ignored.
   *
   * @param struct struct serialization object
   */
  @Override
  public void addSchema(Struct<?> struct) {
    m_inst.addSchema(struct);
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
