// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

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
   * @param name the name
   * @param value the value
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
   * @param struct struct serializer
   */
  public static <T> void log(String name, T value, Struct<T> struct) {
    m_root.log(name, value, struct);
  }

  /**
   * Logs an object with a Protobuf serializer.
   *
   * @param <T> data type
   * @param name the name
   * @param value the value
   * @param proto protobuf serializer
   */
  public static <T> void log(String name, T value, Protobuf<T, ?> proto) {
    m_root.log(name, value, proto);
  }

  /**
   * Logs a generic array.
   *
   * @param name the name
   * @param value the value
   */
  public static void log(String name, Object[] value) {
    m_root.log(name, value);
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
