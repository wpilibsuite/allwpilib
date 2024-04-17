// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.protobuf.ProtobufBuffer;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructBuffer;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Map;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** Helper class for building Sendable dashboard representations. */
public interface SendableTable extends AutoCloseable {
  boolean getBoolean(String name, boolean defaultValue);

  long getInteger(String name, long defaultValue);

  float getFloat(String name, float defaultValue);

  double getDouble(String name, double defaultValue);

  String getString(String name, String defaultValue);

  boolean[] getBooleanArray(String name, boolean[] defaultValue);

  long[] getIntegerArray(String name, long[] defaultValue);

  float[] getFloatArray(String name, float[] defaultValue);

  double[] getDoubleArray(String name, double[] defaultValue);

  String[] getStringArray(String name, String[] defaultValue);

  byte[] getRaw(String name, String typeString, byte[] defaultValue);

  default <T> T getStruct(String name, Struct<T> struct, T defaultValue) {
    byte[] data = getRaw(name, struct.getTypeString(), null);
    if (data == null) {
      return defaultValue;
    }
    // this is inefficient; implementations should cache StructBuffer
    StructBuffer<T> buf = StructBuffer.create(struct);
    return buf.read(data);
  }

  default <T> boolean getStructInto(String name, T out, Struct<T> struct) {
    byte[] data = getRaw(name, struct.getTypeString(), null);
    if (data == null) {
      return false;
    }
    // this is inefficient; implementations should cache StructBuffer
    StructBuffer<T> buf = StructBuffer.create(struct);
    buf.readInto(out, data);
    return true;
  }

  default <T> T getProtobuf(String name, Protobuf<T, ?> proto, T defaultValue) {
    byte[] data = getRaw(name, proto.getTypeString(), null);
    if (data == null) {
      return defaultValue;
    }
    // this is inefficient; implementations should cache ProtobufBuffer
    ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    try {
      return buf.read(data);
    } catch (IOException e) {
      return defaultValue;
    }
  }

  default <T> boolean getProtobufInto(String name, T out, Protobuf<T, ?> proto) {
    byte[] data = getRaw(name, proto.getTypeString(), null);
    if (data == null) {
      return false;
    }
    // this is inefficient; implementations should cache ProtobufBuffer
    ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    try {
      buf.readInto(out, data);
    } catch (IOException e) {
      return false;
    }
    return true;
  }

  void setBoolean(String name, boolean value);

  void setInteger(String name, long value);

  void setFloat(String name, float value);

  void setDouble(String name, double value);

  void setString(String name, String value);

  void setBooleanArray(String name, boolean[] value);

  void setIntegerArray(String name, long[] value);

  void setFloatArray(String name, float[] value);

  void setDoubleArray(String name, double[] value);

  void setStringArray(String name, String[] value);

  void setRaw(String name, String typeString, byte[] value, int start, int len);

  void setRaw(String name, String typeString, ByteBuffer value, int start, int len);

  default <T> void setStruct(String name, T value, Struct<T> struct) {
    addSchema(struct);
    // this is inefficient; implementations should cache StructBuffer
    StructBuffer<T> buf = StructBuffer.create(struct);
    ByteBuffer bb = buf.write(value);
    setRaw(name, struct.getTypeString(), bb, 0, bb.position());
  }

  default <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto) {
    addSchema(proto);
    // this is inefficient; implementations should cache ProtobufBuffer
    ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    try {
      ByteBuffer bb = buf.write(value);
      setRaw(name, proto.getTypeString(), bb, 0, bb.position());
    } catch (IOException e) {
      // ignore
    }
  }

  void publishBoolean(String name, BooleanSupplier supplier);

  void publishInteger(String name, LongSupplier supplier);

  void publishFloat(String name, FloatSupplier supplier);

  void publishDouble(String name, DoubleSupplier supplier);

  void publishString(String name, Supplier<String> supplier);

  void publishBooleanArray(String name, Supplier<boolean[]> supplier);

  void publishIntegerArray(String name, Supplier<long[]> supplier);

  void publishFloatArray(String name, Supplier<float[]> supplier);

  void publishDoubleArray(String name, Supplier<double[]> supplier);

  void publishStringArray(String name, Supplier<String[]> supplier);

  void publishRawBytes(String name, String typeString, Supplier<byte[]> supplier);

  void publishRawBuffer(String name, String typeString, Supplier<ByteBuffer> supplier);

  default <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    publishRawBuffer(
        name,
        struct.getTypeString(),
        () -> {
          return buf.write(supplier.get());
        });
  }

  default <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier) {
    addSchema(proto);
    final ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    publishRawBuffer(
        name,
        proto.getTypeString(),
        () -> {
          try {
            return buf.write(supplier.get());
          } catch (IOException e) {
            return null; // ignore
          }
        });
  }

  BooleanConsumer addBooleanPublisher(String name);

  LongConsumer addIntegerPublisher(String name);

  FloatConsumer addFloatPublisher(String name);

  DoubleConsumer addDoublePublisher(String name);

  Consumer<String> addStringPublisher(String name);

  Consumer<boolean[]> addBooleanArrayPublisher(String name);

  Consumer<long[]> addIntegerArrayPublisher(String name);

  Consumer<float[]> addFloatArrayPublisher(String name);

  Consumer<double[]> addDoubleArrayPublisher(String name);

  Consumer<String[]> addStringArrayPublisher(String name);

  Consumer<byte[]> addRawBytesPublisher(String name, String typeString);

  Consumer<ByteBuffer> addRawBufferPublisher(String name, String typeString);

  default <T> Consumer<T> addStructPublisher(String name, Struct<T> struct) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    final Consumer<ByteBuffer> consumer = addRawBufferPublisher(name, struct.getTypeString());
    return value -> {
      consumer.accept(buf.write(value));
    };
  }

  default <T> Consumer<T> addProtobufPublisher(String name, Protobuf<T, ?> proto) {
    addSchema(proto);
    final ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    final Consumer<ByteBuffer> consumer = addRawBufferPublisher(name, proto.getTypeString());
    return value -> {
      try {
        consumer.accept(buf.write(value));
      } catch (IOException e) {
        // ignore
      }
    };
  }

  void subscribeBoolean(String name, BooleanConsumer consumer);

  void subscribeInteger(String name, LongConsumer consumer);

  void subscribeFloat(String name, FloatConsumer consumer);

  void subscribeDouble(String name, DoubleConsumer consumer);

  void subscribeString(String name, Consumer<String> consumer);

  void subscribeBooleanArray(String name, Consumer<boolean[]> consumer);

  void subscribeIntegerArray(String name, Consumer<long[]> consumer);

  void subscribeFloatArray(String name, Consumer<float[]> consumer);

  void subscribeDoubleArray(String name, Consumer<double[]> consumer);

  void subscribeStringArray(String name, Consumer<String[]> consumer);

  void subscribeRawBytes(String name, String typeString, Consumer<byte[]> consumer);

  default <T> void subscribeStruct(String name, Struct<T> struct, Consumer<T> consumer) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    subscribeRawBytes(
        name,
        struct.getTypeString(),
        data -> {
          consumer.accept(buf.read(data));
        });
  }

  default <T> void subscribeProtobuf(String name, Protobuf<T, ?> proto, Consumer<T> consumer) {
    addSchema(proto);
    final ProtobufBuffer<T, ?> buf = ProtobufBuffer.create(proto);
    subscribeRawBytes(
        name,
        proto.getTypeString(),
        data -> {
          try {
            consumer.accept(buf.read(data));
          } catch (IOException e) {
            // ignore
          }
        });
  }

  <T> SendableTable addSendable(String name, T obj, Sendable<T> sendable);

  SendableTable getChild(String name);

  void setPublishOptions(String name, SendableOption... options);

  void setSubscribeOptions(String name, SendableOption... options);

  /**
   * Gets the current value of a property (as a JSON string).
   *
   * @param name name
   * @param propName property name
   * @return JSON string; "null" if the property does not exist.
   */
  String getProperty(String name, String propName);

  /**
   * Sets a property value.
   *
   * @param name name
   * @param propName property name
   * @param value property value (JSON string)
   * @throws IllegalArgumentException if properties is not parseable as JSON
   */
  void setProperty(String name, String propName, String value);

  /**
   * Deletes a property. Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  void deleteProperty(String name, String propName);

  /**
   * Gets all topic properties as a JSON object string. Each key in the object is the property name,
   * and the corresponding value is the property value.
   *
   * @param name name
   * @return JSON string
   */
  String getProperties(String name);

  /**
   * Updates multiple topic properties. Each key in the passed-in object is the name of the property
   * to add/update, and the corresponding value is the property value to set for that property. Null
   * values result in deletion of the corresponding property.
   *
   * @param name name
   * @param properties map of keys/JSON values to add/update/delete
   * @throws IllegalArgumentException if a properties value is not a JSON object
   */
  void setProperties(String name, Map<String, String> properties);

  void remove(String name);

  /**
   * Return whether this sendable has been published.
   *
   * @return True if it has been published, false if not.
   */
  boolean isPublished();

  /** Update the published values by calling the getters for all properties. */
  void update();

  /** Erases all publishers and subscribers. */
  void clear();

  /**
   * Returns whether there is a data schema already registered with the given name that this
   * instance has published. This does NOT perform a check as to whether the schema has already been
   * published by another node on the network.
   *
   * @param typeString Name (the string passed as the data type for topics using this schema)
   * @return True if schema already registered
   */
  boolean hasSchema(String typeString);

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
  void addSchema(String typeString, String type, byte[] schema);

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
  void addSchema(String typeString, String type, String schema);

  /**
   * Registers a protobuf schema. Duplicate calls to this function with the same name are silently
   * ignored.
   *
   * @param proto protobuf serialization object
   */
  void addSchema(Protobuf<?, ?> proto);

  /**
   * Registers a struct schema. Duplicate calls to this function with the same name are silently
   * ignored.
   *
   * @param struct struct serialization object
   */
  void addSchema(Struct<?> struct);

  /**
   * Return whether close() has been called on this sendable table.
   *
   * @return True if closed, false otherwise.
   */
  boolean isClosed();
}
