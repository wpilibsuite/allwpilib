// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import java.util.Collection;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/**
 * Telemetry sends information from the robot program to dashboards, debug tools, or log files.
 *
 * <p>For more advanced use cases, use the NetworkTables or DataLog APIs directly.
 */
public final class Telemetry {
  /** The root {@link TelemetryTable}. */
  private static final TelemetryTable m_root = TelemetryRegistry.getTable("/");

  private Telemetry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  /**
   * Gets the root telemetry table.
   *
   * @return table
   */
  public static TelemetryTable getTable() {
    return m_root;
  }

  /**
   * Gets a child telemetry table.
   *
   * @param name table name
   * @return table
   */
  public static TelemetryTable getTable(String name) {
    return m_root.getTable(name);
  }

  /**
   * Indicates duplicate values should be preserved. Normally duplicate values are ignored.
   *
   * @param name the name
   */
  public static void keepDuplicates(String name) {
    m_root.keepDuplicates(name);
  }

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param name the name
   * @param key property key
   * @param value property value
   */
  public static void setProperty(String name, String key, String value) {
    m_root.setProperty(name, key, value);
  }

  /**
   * Logs a generic object.
   *
   * <p>For non-null values, the value is dispatched in this order: {@link TelemetryLoggable},
   * {@link org.wpilib.util.struct.StructSerializable}, {@link
   * org.wpilib.util.protobuf.ProtobufSerializable}, boxed primitive and string types, and finally
   * handlers registered with {@link TelemetryRegistry#registerTypeHandler(Class,
   * TelemetryRegistry.TypeHandler)}. {@link TelemetryLoggable} values are logged to a child table
   * named {@code name}. Struct and protobuf serializable values are logged with their public static
   * {@code struct} or {@code proto} serializers. {@link Boolean}, {@link Float}, {@link Double},
   * {@link Byte}, {@link Short}, {@link Integer}, {@link Long}, other {@link Number} values, and
   * {@link String} are logged as primitive or string entries. Generic {@link Number} values other
   * than boxed integral types are logged as double entries. If no registered handler matches, the
   * value falls back to being logged as a string using {@link Object#toString()}. Collection values
   * must be logged with an explicit element type.
   *
   * @param name the name
   * @param value the value; must be non-null
   */
  public static void log(String name, Object value) {
    m_root.log(name, value);
  }

  /**
   * Logs an object with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   */
  public static <T> void log(String name, T value, Struct<? super T> struct) {
    m_root.log(name, value, struct);
  }

  /**
   * Logs an object with a Protobuf serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param proto protobuf serializer for the value type or one of its supertypes
   */
  public static <T> void log(String name, T value, Protobuf<? super T, ?> proto) {
    m_root.log(name, value, proto);
  }

  /**
   * Logs a generic array.
   *
   * <p>{@link Byte} arrays are logged as raw byte arrays.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, Object[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs an array of objects with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   */
  public static <T> void log(String name, T[] value, Struct<? super T> struct) {
    m_root.log(name, value, struct);
  }

  /**
   * Rejects logging a collection without an explicit element type.
   *
   * <p>Use {@link #log(String, Collection, Class)} or {@link #log(String, Collection, Struct)}
   * instead.
   *
   * @param name the name
   * @param value the value
   * @throws CollectionElementTypeRequiredException always
   */
  public static void log(String name, Collection<?> value)
      throws CollectionElementTypeRequiredException {
    throw new CollectionElementTypeRequiredException();
  }

  /**
   * Logs a collection of objects with a Struct serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   */
  public static <T> void log(String name, Collection<T> value, Struct<? super T> struct) {
    m_root.log(name, value, struct);
  }

  /**
   * Logs a collection with an explicit element type.
   *
   * <p>The element type selects the backend representation. {@link Byte} element types are logged
   * as raw byte arrays. {@link Boolean}, {@link Float}, {@link Double}, and {@link String} element
   * types are logged like arrays of the same element type. {@link Number} and other non-integral
   * number element types are logged as double arrays. {@link Short}, {@link Integer}, and {@link
   * Long} element types are logged as integer arrays. Struct-serializable element types are logged
   * with their public static {@code struct} serializer. Other element types fall back to string
   * arrays using {@link Object#toString()}.
   *
   * @param name the name
   * @param value the value
   * @param elementType the collection element type
   */
  public static void log(String name, Collection<?> value, Class<?> elementType) {
    m_root.log(name, value, elementType);
  }

  /**
   * Logs a boolean.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, boolean value) {
    m_root.log(name, value);
  }

  /**
   * Logs a byte.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, byte value) {
    m_root.log(name, value);
  }

  /**
   * Logs a short.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, short value) {
    m_root.log(name, value);
  }

  /**
   * Logs an int.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, int value) {
    m_root.log(name, value);
  }

  /**
   * Logs a long.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, long value) {
    m_root.log(name, value);
  }

  /**
   * Logs a float.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, float value) {
    m_root.log(name, value);
  }

  /**
   * Logs a double.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, double value) {
    m_root.log(name, value);
  }

  /**
   * Logs a String.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, String value) {
    m_root.log(name, value);
  }

  /**
   * Logs a String with custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  public static void log(String name, String value, String typeString) {
    m_root.log(name, value, typeString);
  }

  /**
   * Logs a boolean array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, boolean[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs a short array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, short[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs an int array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, int[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs a long array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, long[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs a float array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, float[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs a double array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, double[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs a String array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, String[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs a raw value (byte array).
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, byte[] value) {
    m_root.log(name, value);
  }

  /**
   * Logs a raw value (byte array) with custom type string.
   *
   * @param name the name
   * @param value the value
   * @param typeString the type string
   */
  public static void log(String name, byte[] value, String typeString) {
    m_root.log(name, value, typeString);
  }
}
