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
import java.util.ArrayList;
import java.util.EnumSet;
import java.util.List;
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
public class NetworkSendableTable implements SendableTable {
  private static final char PATH_SEPARATOR = '/';

  private final String m_path;
  private final String m_pathWithSep;
  private final NetworkTableInstance m_inst;

  private static class TopicData {
    TopicData(Topic topic) {
      m_topic = topic;
    }

    synchronized void close() {
      if (m_publisher != null) {
        m_publisher.close();
      }
      if (m_subscriber != null) {
        m_subscriber.close();
      }
      for (Integer listener : m_listeners) {
        m_topic.m_inst.removeListener(listener);
      }
    }

    synchronized GenericPublisher publish(String typeString) {
      if (m_publisher != null) {
        return m_publisher;
      }
      if (typeString == null) {
        this.m_typeString = typeString;
      }
      m_publisher = m_topic.genericPublish(m_typeString, m_pubOptions);
      return m_publisher;
    }

    synchronized GenericSubscriber subscribe(String typeString) {
      if (m_subscriber != null) {
        return m_subscriber;
      }
      if (typeString == null) {
        this.m_typeString = typeString;
      }
      m_subscriber = m_topic.genericSubscribe(m_typeString, m_subOptions);
      return m_subscriber;
    }

    synchronized GenericPublisher getPublisher() {
      if (m_publisher != null && m_publisher.isValid()) {
        return m_publisher;
      } else {
        return null;
      }
    }

    private PubSubOption[] fromSendableOptions(SendableOption... options) {
      ArrayList<PubSubOption> out = new ArrayList<>();
      for (SendableOption option : options) {
        switch (option.getKind()) {
          case kPeriodic:
            out.add(PubSubOption.periodic(option.getDoubleValue()));
            break;
          case kTypeString:
            m_typeString = option.getStringValue();
            break;
          case kSendAll:
            out.add(PubSubOption.sendAll(option.getBooleanValue()));
            break;
          case kPollStorage:
            out.add(PubSubOption.pollStorage(option.getIntValue()));
            break;
          case kKeepDuplicates:
            out.add(PubSubOption.keepDuplicates(option.getBooleanValue()));
            break;
          case kDisableRemote:
            out.add(PubSubOption.disableRemote(option.getBooleanValue()));
            break;
          case kDisableLocal:
            out.add(PubSubOption.disableLocal(option.getBooleanValue()));
            break;
          default:
            break;
        }
      }
      return out.toArray(new PubSubOption[0]);
    }

    synchronized void setPublishOptions(SendableOption... options) {
      m_pubOptions = fromSendableOptions(options);
      if (m_publisher != null) {
        m_publisher.close();
        m_publisher = m_topic.genericPublish(m_typeString, m_pubOptions);
      }
    }

    synchronized void setSubscribeOptions(SendableOption... options) {
      m_pubOptions = fromSendableOptions(options);
      if (m_subscriber != null) {
        m_subscriber.close();
        m_subscriber = m_topic.genericSubscribe(m_typeString, m_pubOptions);
      }
    }

    synchronized void addValueListener(String typeString, Consumer<NetworkTableValue> cb) {
      if (m_subscriber == null) {
        subscribe(typeString);
      }
      m_listeners.add(m_topic.getInstance().addListener(m_subscriber, EnumSet.of(NetworkTableEvent.Kind.kValueAll), e -> {
        if (e.valueData != null) {
          cb.accept(e.valueData.value);
        }
      }));
    }

    void setPolledUpdate(Consumer<GenericPublisher> consumer) {
      m_polledUpdate.set(consumer);
    }

    Consumer<GenericPublisher> getPolledUpdate() {
      return m_polledUpdate.get();
    }

    final Topic m_topic;
    GenericPublisher m_publisher;
    GenericSubscriber m_subscriber;
    String m_typeString;
    PubSubOption[] m_pubOptions = new PubSubOption[0];
    PubSubOption[] m_subOptions = new PubSubOption[0];
    StructBuffer<?> m_structBuffer;
    ProtobufBuffer<?, ?> m_protobufBuffer;
    final AtomicReference<Consumer<GenericPublisher>> m_polledUpdate = new AtomicReference<>();
    final List<Integer> m_listeners = new ArrayList<>();
  }
  private final ConcurrentMap<String, TopicData> m_topics = new ConcurrentHashMap<>();
  private final ConcurrentMap<String, NetworkSendableTable> m_tables = new ConcurrentHashMap<>();
  private Consumer<SendableTable> m_closeSendable;
  private boolean m_closed;

  private TopicData getTopicData(String name) {
    return m_topics.computeIfAbsent(name, k -> new TopicData(m_inst.getTopic(m_pathWithSep + name)));
  }

  private TopicData publish(String name, String typeString) {
    TopicData data = getTopicData(name);
    data.publish(typeString);
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
    return getTopicData(name).subscribe("boolean").getBoolean(defaultValue);
  }

  @Override
  public long getInteger(String name, long defaultValue) {
    return getTopicData(name).subscribe("int").getInteger(defaultValue);
  }

  @Override
  public float getFloat(String name, float defaultValue) {
    return getTopicData(name).subscribe("float").getFloat(defaultValue);
  }

  @Override
  public double getDouble(String name, double defaultValue) {
    return getTopicData(name).subscribe("double").getDouble(defaultValue);
  }

  @Override
  public String getString(String name, String defaultValue) {
    return getTopicData(name).subscribe("string").getString(defaultValue);
  }

  @Override
  public boolean[] getBooleanArray(String name, boolean[] defaultValue) {
    return getTopicData(name).subscribe("boolean[]").getBooleanArray(defaultValue);
  }

  @Override
  public long[] getIntegerArray(String name, long[] defaultValue) {
    return getTopicData(name).subscribe("int[]").getIntegerArray(defaultValue);
  }

  @Override
  public float[] getFloatArray(String name, float[] defaultValue) {
    return getTopicData(name).subscribe("float[]").getFloatArray(defaultValue);
  }

  @Override
  public double[] getDoubleArray(String name, double[] defaultValue) {
    return getTopicData(name).subscribe("double[]").getDoubleArray(defaultValue);
  }

  @Override
  public String[] getStringArray(String name, String[] defaultValue) {
    return getTopicData(name).subscribe("string[]").getStringArray(defaultValue);
  }

  @Override
  public byte[] getRaw(String name, String typeString, byte[] defaultValue) {
    return getTopicData(name).subscribe(typeString).getRaw(defaultValue);
  }

  @Override
  public <T> T getStruct(String name, Struct<T> struct, T defaultValue) {
    TopicData td = getTopicData(name);
    byte[] data = td.subscribe(struct.getTypeString()).getRaw(null);
    if (data == null) {
      return defaultValue;
    }
    if (td.m_structBuffer == null) {
      td.m_structBuffer = StructBuffer.create(struct);
    } else if (td.m_structBuffer.getStruct() != struct) {
      return defaultValue;
    }
    @SuppressWarnings("unchecked")
    StructBuffer<T> buf = ((StructBuffer<T>) td.m_structBuffer);
    return buf.read(data);
  }

  @Override
  public <T> boolean getStructInto(String name, T out, Struct<T> struct) {
    TopicData td = getTopicData(name);
    synchronized (td) {
      if (td.m_subscriber == null) {
        td.subscribe(struct.getTypeString());
        addSchema(struct);
      }
      byte[] data = td.m_subscriber.getRaw(null);
      if (data == null) {
        return false;
      }
      if (td.m_structBuffer == null) {
        td.m_structBuffer = StructBuffer.create(struct);
      } else if (td.m_structBuffer.getStruct() != struct) {
        return false;
      }
      @SuppressWarnings("unchecked")
      StructBuffer<T> buf = ((StructBuffer<T>) td.m_structBuffer);
      buf.readInto(out, data);
      return true;
    }
  }

  @Override
  public <T> T getProtobuf(String name, Protobuf<T, ?> proto, T defaultValue) {
    TopicData td = getTopicData(name);
    synchronized (td) {
      if (td.m_subscriber == null) {
        td.subscribe(proto.getTypeString());
        addSchema(proto);
      }
      byte[] data = td.m_subscriber.getRaw(null);
      if (data == null) {
        return defaultValue;
      }
      if (td.m_protobufBuffer == null) {
        td.m_protobufBuffer = ProtobufBuffer.create(proto);
      } else if (td.m_protobufBuffer.getProto() != proto) {
        return defaultValue;
      }
      @SuppressWarnings("unchecked")
      ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.m_protobufBuffer);
      try {
        return buf.read(data);
      } catch (IOException e) {
        return defaultValue;
      }
    }
  }

  @Override
  public <T> boolean getProtobufInto(String name, T out, Protobuf<T, ?> proto) {
    TopicData td = getTopicData(name);
    byte[] data = td.subscribe(proto.getTypeString()).getRaw(null);
    if (data == null) {
      return false;
    }
    if (td.m_protobufBuffer == null) {
      td.m_protobufBuffer = ProtobufBuffer.create(proto);
    } else if (td.m_protobufBuffer.getProto() != proto) {
      return false;
    }
    @SuppressWarnings("unchecked")
    ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.m_protobufBuffer);
    try {
      buf.readInto(out, data);
    } catch (IOException e) {
      return false;
    }
    return true;
  }

  @Override
  public void setBoolean(String name, boolean value) {
    getTopicData(name).publish("boolean").setBoolean(value);
  }

  @Override
  public void setInteger(String name, long value) {
    getTopicData(name).publish("int").setInteger(value);
  }

  @Override
  public void setFloat(String name, float value) {
    getTopicData(name).publish("float").setFloat(value);
  }

  @Override
  public void setDouble(String name, double value) {
    getTopicData(name).publish("double").setDouble(value);
  }

  @Override
  public void setString(String name, String value) {
    getTopicData(name).publish("string").setString(value);
  }

  @Override
  public void setBooleanArray(String name, boolean[] value) {
    getTopicData(name).publish("boolean[]").setBooleanArray(value);
  }

  @Override
  public void setIntegerArray(String name, long[] value) {
    getTopicData(name).publish("int[]").setIntegerArray(value);
  }

  @Override
  public void setFloatArray(String name, float[] value) {
    getTopicData(name).publish("float[]").setFloatArray(value);
  }

  @Override
  public void setDoubleArray(String name, double[] value) {
    getTopicData(name).publish("double[]").setDoubleArray(value);
  }

  @Override
  public void setStringArray(String name, String[] value) {
    getTopicData(name).publish("string[]").setStringArray(value);
  }

  @Override
  public void setRaw(String name, String typeString, byte[] value, int start, int len) {
    getTopicData(name).publish(typeString).setRaw(value, start, len);
  }

  @Override
  public void setRaw(String name, String typeString, ByteBuffer value, int start, int len) {
    getTopicData(name).publish(typeString).setRaw(value, start, len);
  }

  @Override
  public <T> void setStruct(String name, T value, Struct<T> struct) {
    TopicData td = getTopicData(name);
    synchronized (td) {
      if (td.m_publisher == null) {
        td.publish(struct.getTypeString());
        addSchema(struct);
      }
      if (td.m_structBuffer == null) {
        td.m_structBuffer = StructBuffer.create(struct);
      } else if (td.m_structBuffer.getStruct() != struct) {
        return;
      }
      @SuppressWarnings("unchecked")
      StructBuffer<T> buf = ((StructBuffer<T>) td.m_structBuffer);
      ByteBuffer bb = buf.write(value);
      td.m_publisher.setRaw(bb, 0, bb.position());
    }
  }

  @Override
  public <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto) {
    TopicData td = getTopicData(name);
    synchronized (td) {
      if (td.m_publisher == null) {
        td.publish(proto.getTypeString());
        addSchema(proto);
      }
      if (td.m_protobufBuffer == null) {
        td.m_protobufBuffer = ProtobufBuffer.create(proto);
      } else if (td.m_protobufBuffer.getProto() != proto) {
        return;
      }
      @SuppressWarnings("unchecked")
      ProtobufBuffer<T, ?> buf = ((ProtobufBuffer<T, ?>) td.m_protobufBuffer);
      try {
        ByteBuffer bb = buf.write(value);
        td.m_publisher.setRaw(bb, 0, bb.position());
      } catch (IOException e) {
        // ignore
      }
    }
  }

  @Override
  public void publishBoolean(String name, BooleanSupplier supplier) {
    TopicData data = publish(name, "boolean");
    data.setPolledUpdate(pub -> { pub.setBoolean(supplier.getAsBoolean()); });
  }

  @Override
  public void publishInteger(String name, LongSupplier supplier) {
    TopicData data = publish(name, "int");
    data.setPolledUpdate(pub -> { pub.setInteger(supplier.getAsLong()); });
  }

  @Override
  public void publishFloat(String name, FloatSupplier supplier) {
    TopicData data = publish(name, "float");
    data.setPolledUpdate(pub -> { pub.setFloat(supplier.getAsFloat()); });
  }

  @Override
  public void publishDouble(String name, DoubleSupplier supplier) {
    TopicData data = publish(name, "double");
    data.setPolledUpdate(pub -> { pub.setDouble(supplier.getAsDouble()); });
  }

  @Override
  public void publishString(String name, Supplier<String> supplier) {
    TopicData data = publish(name, "string");
    data.setPolledUpdate(pub -> { pub.setString(supplier.get()); });
  }

  @Override
  public void publishBooleanArray(String name, Supplier<boolean[]> supplier) {
    TopicData data = publish(name, "boolean[]");
    data.setPolledUpdate(pub -> { pub.setBooleanArray(supplier.get()); });
  }

  @Override
  public void publishIntegerArray(String name, Supplier<long[]> supplier) {
    TopicData data = publish(name, "int[]");
    data.setPolledUpdate(pub -> { pub.setIntegerArray(supplier.get()); });
  }

  @Override
  public void publishFloatArray(String name, Supplier<float[]> supplier) {
    TopicData data = publish(name, "float[]");
    data.setPolledUpdate(pub -> { pub.setFloatArray(supplier.get()); });
  }

  @Override
  public void publishDoubleArray(String name, Supplier<double[]> supplier) {
    TopicData data = publish(name, "double[]");
    data.setPolledUpdate(pub -> { pub.setDoubleArray(supplier.get()); });
  }

  @Override
  public void publishStringArray(String name, Supplier<String[]> supplier) {
    TopicData data = publish(name, "string[]");
    data.setPolledUpdate(pub -> { pub.setStringArray(supplier.get()); });
  }

  @Override
  public void publishRawBytes(String name, String typeString, Supplier<byte[]> supplier) {
    TopicData data = publish(name, typeString);
    data.setPolledUpdate(pub -> { pub.setRaw(supplier.get()); });
  }

  @Override
  public void publishRawBuffer(String name, String typeString, Supplier<ByteBuffer> supplier) {
    TopicData data = publish(name, typeString);
    data.setPolledUpdate(pub -> { pub.setRaw(supplier.get()); });
  }

  @Override
  public <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    TopicData td = getTopicData(name);
    td.publish(struct.getTypeString());
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    td.setPolledUpdate(pub -> { pub.setRaw(buf.write(supplier.get())); });
  }

  @Override
  public <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier) {
    TopicData td = getTopicData(name);
    td.publish(proto.getTypeString());
    addSchema(proto);
    final ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    td.setPolledUpdate(pub -> {
      try {
        pub.setRaw(buf.write(supplier.get()));
      } catch (IOException e) {
        return; // ignore
      }
    });
  }

  @Override
  public BooleanConsumer addBooleanPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "boolean"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setBoolean(value);
      }
    };
  }

  @Override
  public LongConsumer addIntegerPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "int"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setInteger(value);
      }
    };
  }

  @Override
  public FloatConsumer addFloatPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "float"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setFloat(value);
      }
    };
  }

  @Override
  public DoubleConsumer addDoublePublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "double"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setDouble(value);
      }
    };
  }

  @Override
  public Consumer<String> addStringPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "string"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setString(value);
      }
    };
  }

  @Override
  public Consumer<boolean[]> addBooleanArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "boolean[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setBooleanArray(value);
      }
    };
  }

  @Override
  public Consumer<long[]> addIntegerArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "int[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setIntegerArray(value);
      }
    };
  }

  @Override
  public Consumer<float[]> addFloatArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "float[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setFloatArray(value);
      }
    };
  }

  @Override
  public Consumer<double[]> addDoubleArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "double[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setDoubleArray(value);
      }
    };
  }

  @Override
  public Consumer<String[]> addStringArrayPublisher(String name) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, "string[]"));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setStringArray(value);
      }
    };
  }

  @Override
  public Consumer<byte[]> addRawBytesPublisher(String name, String typeString) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, typeString));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setRaw(value);
      }
    };
  }

  @Override
  public Consumer<ByteBuffer> addRawBufferPublisher(String name, String typeString) {
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, typeString));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setRaw(value);
      }
    };
  }

  @Override
  public <T> Consumer<T> addStructPublisher(String name, Struct<T> struct) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, struct.getTypeString()));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        publisher.setRaw(buf.write(value));
      }
    };
  }

  @Override
  public <T> Consumer<T> addProtobufPublisher(String name, Protobuf<T, ?> proto) {
    addSchema(proto);
    final ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    final WeakReference<TopicData> dataRef = new WeakReference<>(publish(name, proto.getTypeString()));
    return value -> {
      TopicData data = dataRef.get();
      if (data == null) {
        return;
      }
      GenericPublisher publisher = data.getPublisher();
      if (publisher != null) {
        try {
          publisher.setRaw(buf.write(value));
        } catch (IOException e) {
          // ignore
        }
      }
    };
  }

  @Override
  public void subscribeBoolean(String name, BooleanConsumer consumer) {
    getTopicData(name).addValueListener("boolean", value -> {
      if (value.isBoolean()) {
        consumer.accept(value.getBoolean());
      }
    });
  }

  @Override
  public void subscribeInteger(String name, LongConsumer consumer) {
    getTopicData(name).addValueListener("int", value -> {
      if (value.isInteger()) {
        consumer.accept(value.getInteger());
      }
    });
  }

  @Override
  public void subscribeFloat(String name, FloatConsumer consumer) {
    getTopicData(name).addValueListener("float", value -> {
      if (value.isFloat()) {
        consumer.accept(value.getFloat());
      }
    });
  }

  @Override
  public void subscribeDouble(String name, DoubleConsumer consumer) {
    getTopicData(name).addValueListener("double", value -> {
      if (value.isDouble()) {
        consumer.accept(value.getDouble());
      }
    });
  }

  @Override
  public void subscribeString(String name, Consumer<String> consumer) {
    getTopicData(name).addValueListener("string", value -> {
      if (value.isString()) {
        consumer.accept(value.getString());
      }
    });
  }

  @Override
  public void subscribeBooleanArray(String name, Consumer<boolean[]> consumer) {
    getTopicData(name).addValueListener("boolean[]", value -> {
      if (value.isBooleanArray()) {
        consumer.accept(value.getBooleanArray());
      }
    });
  }

  @Override
  public void subscribeIntegerArray(String name, Consumer<long[]> consumer) {
    getTopicData(name).addValueListener("int[]", value -> {
      if (value.isIntegerArray()) {
        consumer.accept(value.getIntegerArray());
      }
    });
  }

  @Override
  public void subscribeFloatArray(String name, Consumer<float[]> consumer) {
    getTopicData(name).addValueListener("float[]", value -> {
      if (value.isFloatArray()) {
        consumer.accept(value.getFloatArray());
      }
    });
  }

  @Override
  public void subscribeDoubleArray(String name, Consumer<double[]> consumer) {
    getTopicData(name).addValueListener("double[]", value -> {
      if (value.isDoubleArray()) {
        consumer.accept(value.getDoubleArray());
      }
    });
  }

  @Override
  public void subscribeStringArray(String name, Consumer<String[]> consumer) {
    getTopicData(name).addValueListener("string[]", value -> {
      if (value.isStringArray()) {
        consumer.accept(value.getStringArray());
      }
    });
  }

  @Override
  public void subscribeRawBytes(String name, String typeString, Consumer<byte[]> consumer) {
    getTopicData(name).addValueListener(typeString, value -> {
      if (value.isRaw()) {
        consumer.accept(value.getRaw());
      }
    });
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
    getTopicData(name).setPublishOptions(options);
  }

  @Override
  public void setSubscribeOptions(String name, SendableOption... options) {
    getTopicData(name).setSubscribeOptions(options);
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
    return getTopicData(name).m_topic.getProperty(propName);
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
    getTopicData(name).m_topic.setProperty(propName, value);
  }

  /**
   * Deletes a property. Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  @Override
  public void deleteProperty(String name, String propName) {
    getTopicData(name).m_topic.deleteProperty(propName);
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
    return getTopicData(name).m_topic.getProperties();
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
    // convert properties to a single JSON string
    StringBuilder jsonProperties = new StringBuilder();
    jsonProperties.append('{');
    properties.forEach((k, v) -> {
      jsonProperties.append('"');
      jsonProperties.append(k.replace("\"", "\\\""));
      jsonProperties.append("\":");
      jsonProperties.append(v);
      jsonProperties.append(',');
    });
    // replace the trailing comma with a }
    jsonProperties.setCharAt(jsonProperties.length() - 1, '}');
    getTopicData(name).m_topic.setProperties(jsonProperties.toString());
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
    if (isClosed()) {
      return;
    }
    for (TopicData data : m_topics.values()) {
      GenericPublisher publisher = data.getPublisher();
      Consumer<GenericPublisher> consumer = data.getPolledUpdate();
      if (publisher != null && consumer != null) {
        consumer.accept(publisher);
      }
    }
    for (NetworkSendableTable table : m_tables.values()) {
      if (!table.isClosed()) {
        table.update();
      }
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
   * @param typeString Name (the string passed as the data type for topics using this schema)
   * @return True if schema already registered
   */
  @Override
  public boolean hasSchema(String typeString) {
    return m_inst.hasSchema(typeString);
  }

  /**
   * Registers a data schema. Data schemas provide information for how a certain data type string
   * can be decoded. The type string of a data schema indicates the type of the schema itself (e.g.
   * "protobuf" for protobuf schemas, "struct" for struct schemas, etc). In NetworkTables, schemas
   * are published just like normal topics, with the name being generated from the provided name:
   * "/.schema/name". Duplicate calls to this function with the same name are silently ignored.
   *
   * @param typeString Name (the string passed as the data type for topics using this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  @Override
  public void addSchema(String typeString, String type, byte[] schema) {
    m_inst.addSchema(typeString, type, schema);
  }

  /**
   * Registers a data schema. Data schemas provide information for how a certain data type string
   * can be decoded. The type string of a data schema indicates the type of the schema itself (e.g.
   * "protobuf" for protobuf schemas, "struct" for struct schemas, etc). In NetworkTables, schemas
   * are published just like normal topics, with the name being generated from the provided name:
   * "/.schema/name". Duplicate calls to this function with the same name are silently ignored.
   *
   * @param typeString Name (the string passed as the data type for topics using this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   */
  @Override
  public void addSchema(String typeString, String type, String schema) {
    m_inst.addSchema(typeString, type, schema);
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
