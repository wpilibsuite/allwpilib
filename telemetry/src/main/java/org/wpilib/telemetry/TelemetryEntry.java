// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/**
 * Interface for individual telemetry entries. Intended for use by backends; most users should use
 * the TelemetryTable class.
 *
 * <p>Implementations must not throw from logging, metadata, or discard-checking methods.
 * Recoverable failures should be reported through {@link TelemetryRegistry#reportWarning(String,
 * String)} and skipped.
 *
 * <p>Methods on the same entry may be called concurrently. Implementations must copy mutable input
 * they retain after a method returns.
 */
public interface TelemetryEntry {
  /**
   * Checks if this entry discards data.
   *
   * @return true if this entry discards logged data
   */
  default boolean isDiscard() {
    return false;
  }

  /** Indicates duplicate values should be preserved. Normally duplicate values are ignored. */
  void keepDuplicates();

  /**
   * Sets property for a value. Properties are stored as a key/value map.
   *
   * @param key property key
   * @param value property value; must be a valid JSON value string (e.g. quoted string, number,
   *     true, false, null, object, or array)
   */
  void setProperty(String key, String value);

  /**
   * Logs an object with struct serialization.
   *
   * @param <T> data type
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  <T> void logStruct(T value, Struct<? super T> struct, long timestamp);

  /**
   * Logs an object with protobuf serialization.
   *
   * @param <T> data type
   * @param value the value
   * @param proto protobuf serializer for the value type or one of its supertypes
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp);

  /**
   * Logs an array of objects with struct serialization.
   *
   * @param <T> data type
   * @param value the value
   * @param struct struct serializer for the value type or one of its supertypes
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp);

  /**
   * Logs a boolean.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logBoolean(boolean value, long timestamp);

  /**
   * Logs a byte.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  default void logByte(byte value, long timestamp) {
    logLong(value, timestamp);
  }

  /**
   * Logs a short.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  default void logShort(short value, long timestamp) {
    logLong(value, timestamp);
  }

  /**
   * Logs an int.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  default void logInt(int value, long timestamp) {
    logLong(value, timestamp);
  }

  /**
   * Logs a long.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logLong(long value, long timestamp);

  /**
   * Logs a float.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logFloat(float value, long timestamp);

  /**
   * Logs a double.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logDouble(double value, long timestamp);

  /**
   * Logs a String.
   *
   * @param value the value
   * @param typeString the type string
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logString(String value, String typeString, long timestamp);

  /**
   * Logs a boolean array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logBooleanArray(boolean[] value, long timestamp);

  /**
   * Logs a short array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logShortArray(short[] value, long timestamp);

  /**
   * Logs an int array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logIntArray(int[] value, long timestamp);

  /**
   * Logs a long array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logLongArray(long[] value, long timestamp);

  /**
   * Logs a float array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logFloatArray(float[] value, long timestamp);

  /**
   * Logs a double array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logDoubleArray(double[] value, long timestamp);

  /**
   * Logs a String array.
   *
   * @param value the value
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logStringArray(String[] value, long timestamp);

  /**
   * Logs a byte array (raw value).
   *
   * @param value the value
   * @param typeString the type string
   * @param timestamp timestamp in nanoseconds in the same time base as {@link
   *     org.wpilib.util.WPIUtilJNI#now() WPIUtilJNI.now()}, or 0 to use the current time
   */
  void logRaw(byte[] value, String typeString, long timestamp);
}
