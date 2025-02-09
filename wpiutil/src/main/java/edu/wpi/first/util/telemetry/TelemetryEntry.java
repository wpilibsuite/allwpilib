// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import us.hebi.quickbuf.ProtoMessage;

public interface TelemetryEntry {
  /** Indicates duplicate values should be preserved. Normally duplicate values are ignored. */
  void keepDuplicates();

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param key property key
   * @param value property value
   */
  void setProperty(String key, String value);

  /**
   * Sets custom data type for a value. Generally not necessary.
   *
   * @param typeString type string
   */
  void setTypeString(String typeString);

  /**
   * Logs an object with struct serialization.
   *
   * @param value the value
   * @param struct struct serializer
   */
  void logStruct(Object value, Struct<?> struct);

  /**
   * Logs an object with protobuf serialization.
   *
   * @param value the value
   * @param protobuf protobuf serializer
   */
  <MessageType extends ProtoMessage<?>> void logProtobuf(
      Object value, Protobuf<?, MessageType> protobuf);

  /**
   * Logs an array of objects with struct serialization.
   *
   * @param value the value
   * @param struct struct serializer
   */
  void logStructArray(Object[] value, Struct<?> struct);

  /**
   * Logs a boolean.
   *
   * @param value the value
   */
  void logBoolean(boolean value);

  /**
   * Logs a byte.
   *
   * @param value the value
   */
  default void logByte(byte value) {
    logLong(value);
  }

  /**
   * Logs a short.
   *
   * @param value the value
   */
  default void logShort(short value) {
    logLong(value);
  }

  /**
   * Logs an int.
   *
   * @param value the value
   */
  default void logInt(int value) {
    logLong(value);
  }

  /**
   * Logs a long.
   *
   * @param value the value
   */
  void logLong(long value);

  /**
   * Logs a float.
   *
   * @param value the value
   */
  void logFloat(float value);

  /**
   * Logs a double.
   *
   * @param value the value
   */
  void logDouble(double value);

  /**
   * Logs a String.
   *
   * @param value the value
   */
  void logString(String value);

  /**
   * Logs a boolean array.
   *
   * @param value the value
   */
  void logBooleanArray(boolean[] value);

  /**
   * Logs a byte array (raw value).
   *
   * @param value the value
   */
  void logByteArray(byte[] value);

  /**
   * Logs a short array.
   *
   * @param value the value
   */
  void logShortArray(short[] value);

  /**
   * Logs an int array.
   *
   * @param value the value
   */
  void logIntArray(int[] value);

  /**
   * Logs a long array.
   *
   * @param value the value
   */
  void logLongArray(long[] value);

  /**
   * Logs a float array.
   *
   * @param value the value
   */
  void logFloatArray(float[] value);

  /**
   * Logs a double array.
   *
   * @param value the value
   */
  void logDoubleArray(double[] value);

  /**
   * Logs a String array.
   *
   * @param value the value
   */
  void logStringArray(String[] value);
}
