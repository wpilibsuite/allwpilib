// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import edu.wpi.first.util.struct.StructBuffer;

import java.nio.ByteBuffer;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** Helper class for building Sendable dashboard representations. */
public interface SendableTable extends AutoCloseable {
  void setBoolean(String name, boolean value);

  void publishBoolean(String name, BooleanSupplier supplier);

  BooleanConsumer publishBoolean(String name);

  void subscribeBoolean(String name, BooleanConsumer consumer);

  void setInt(String name, long value);

  void publishInt(String name, LongSupplier supplier);

  LongConsumer publishInt(String name);

  void subscribeInt(String name, LongConsumer consumer);

  void setFloat(String name, float value);

  void publishFloat(String name, FloatSupplier supplier);

  FloatConsumer publishFloat(String name);

  void subscribeFloat(String name, FloatConsumer consumer);

  void setDouble(String name, double value);

  void publishDouble(String name, DoubleSupplier supplier);

  DoubleConsumer publishDouble(String name);

  void subscribeDouble(String name, DoubleConsumer consumer);

  void setString(String name, String value);

  void publishString(String name, Supplier<String> supplier);

  Consumer<String> publishString(String name);

  void subscribeString(String name, Consumer<String> consumer);

  void setRaw(String name, String typeString, byte[] value, int start, int len);

  void setRaw(String name, String typeString, ByteBuffer value, int start, int len);

  void publishRaw(String name, String typeString, Supplier<byte[]> supplier);

  void publishRawBB(String name, String typeString, Supplier<ByteBuffer> supplier);

  Consumer<byte[]> publishRaw(String name, String typeString);

  Consumer<ByteBuffer> publishRawBB(String name, String typeString);

  void subscribeRaw(String name, String typeString, Consumer<byte[]> consumer);

  default <T> void setStruct(String name, T value, Struct<T> struct) {
    addSchema(struct);
    // this is inefficient; implementations should cache StructBuffer
    StructBuffer<T> buf = StructBuffer.create(struct);
    ByteBuffer bb = buf.write(value);
    setRaw(name, struct.getTypeString(), bb, 0, bb.position());
  }

  default <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    publishRawBB(name, struct.getTypeString(), () -> {
      return buf.write(supplier.get());
    });
  }

  default <T> Consumer<T> publishStruct(String name, Struct<T> struct) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    final Consumer<ByteBuffer> consumer = publishRawBB(name, struct.getTypeString());
    return (T value) -> {
      consumer.accept(buf.write(value));
    };
  }

  default <T> void subscribeStruct(String name, Struct<T> struct, Consumer<T> consumer) {
    addSchema(struct);
    final StructBuffer<T> buf = StructBuffer.create(struct);
    subscribeRaw(name, struct.getTypeString(), (data) -> {
      consumer.accept(buf.read(data));
    });
  }

  <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto);

  <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier);

  <T> Consumer<T> publishProtobuf(String name, Protobuf<T, ?> proto);

  <T> void subscribeProtobuf(String name, Protobuf<T, ?> proto, Consumer<T> consumer);

  <T> SendableTable addSendable(String name, T obj, Sendable<T> sendable);

  // SendableTable getSendable(String name);

  SendableTable addChild(String name);

  void setPublishOptions(SendableOption... options);

  void setSubscribeOptions(SendableOption... options);

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
   * @param properties JSON object string with keys to add/update/delete
   * @throws IllegalArgumentException if properties is not a JSON object
   */
  void setProperties(String name, String properties);

  void erase(String name);

  /**
   * Return whether this sendable has been published.
   *
   * @return True if it has been published, false if not.
   */
  boolean isPublished();

  /** Update the published values by calling the getters for all properties. */
  void update();

  /** Clear properties. */
  void clear();

  /**
   * Returns whether there is a data schema already registered with the given name that this
   * instance has published. This does NOT perform a check as to whether the schema has already
   * been published by another node on the network.
   *
   * @param name Name (the string passed as the data type for topics using this schema)
   * @return True if schema already registered
   */
  boolean hasSchema(String name);

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
  void addSchema(String name, String type, byte[] schema);

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
  void addSchema(String name, String type, String schema);

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
