// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable;

import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** Helper class for building Sendable dashboard representations. */
public interface SendableBuilder extends AutoCloseable {
  /** The backend kinds used for the sendable builder. */
  enum BackendKind {
    /** Unknown. */
    kUnknown,

    /** NetworkTables. */
    kNetworkTables
  }

  /**
   * Set the string representation of the named data type that will be used by the smart dashboard
   * for this sendable.
   *
   * @param type data type
   */
  void setSmartDashboardType(String type);

  /**
   * Set a flag indicating if this Sendable should be treated as an actuator. By default, this flag
   * is false.
   *
   * @param value true if actuator, false if not
   */
  void setActuator(boolean value);

  /**
   * Set the function that should be called to set the Sendable into a safe state. This is called
   * when entering and exiting Live Window mode.
   *
   * @param func function
   */
  void setSafeState(Runnable func);

  /**
   * Add a read only boolean property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addBooleanProperty(String key, BooleanSupplier getter) {
    addBooleanProperty(key, getter, null);
  }

  /**
   * Add a boolean property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addBooleanProperty(String key, BooleanSupplier getter, BooleanConsumer setter);

  /**
   * Add a constant boolean property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstBoolean(String key, boolean value);

  /**
   * Add a read only integer property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addIntegerProperty(String key, LongSupplier getter) {
    addIntegerProperty(key, getter, null);
  }

  /**
   * Add an integer property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addIntegerProperty(String key, LongSupplier getter, LongConsumer setter);

  /**
   * Add a constant integer property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstInteger(String key, long value);

  /**
   * Add a read only float property. The setter is null.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addFloatProperty(String key, FloatSupplier getter) {
    addFloatProperty(key, getter, null);
  }

  /**
   * Add a float property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addFloatProperty(String key, FloatSupplier getter, FloatConsumer setter);

  /**
   * Add a constant float property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstFloat(String key, float value);

  /**
   * Add a read only double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addDoubleProperty(String key, DoubleSupplier getter) {
    addDoubleProperty(key, getter, null);
  }
  ;

  /**
   * Add a double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addDoubleProperty(String key, DoubleSupplier getter, DoubleConsumer setter);

  /**
   * Add a constant double property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstDouble(String key, double value);

  /**
   * Add a read only string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addStringProperty(String key, Supplier<String> getter) {
    addStringProperty(key, getter, null);
  }

  /**
   * Add a string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addStringProperty(String key, Supplier<String> getter, Consumer<String> setter);

  /**
   * Add a constant string property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstString(String key, String value);

  /**
   * Add a read only boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addBooleanArrayProperty(String key, Supplier<boolean[]> getter) {
    addBooleanArrayProperty(key, getter, null);
  }

  /**
   * Add a boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addBooleanArrayProperty(String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter);

  /**
   * Add a constant boolean array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstBooleanArray(String key, boolean[] value);

  /**
   * Add a read only integer array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addIntegerArrayProperty(String key, Supplier<long[]> getter) {
    addIntegerArrayProperty(key, getter, null);
  }

  /**
   * Add an integer array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addIntegerArrayProperty(String key, Supplier<long[]> getter, Consumer<long[]> setter);

  /**
   * Add a constant integer property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstIntegerArray(String key, long[] value);

  /**
   * Add a read only float array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addFloatArrayProperty(String key, Supplier<float[]> getter) {
    addFloatArrayProperty(key, getter, null);
  }

  /**
   * Add a float array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addFloatArrayProperty(String key, Supplier<float[]> getter, Consumer<float[]> setter);

  /**
   * Add a constant float array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstFloatArray(String key, float[] value);

  /**
   * Add a read only double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addDoubleArrayProperty(String key, Supplier<double[]> getter) {
    addDoubleArrayProperty(key, getter, null);
  }

  /**
   * Add a double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addDoubleArrayProperty(String key, Supplier<double[]> getter, Consumer<double[]> setter);

  /**
   * Add a constant double array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstDoubleArray(String key, double[] value);

  /**
   * Add a read only string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   */
  default void addStringArrayProperty(String key, Supplier<String[]> getter) {
    addStringArrayProperty(key, getter, null);
  }

  /**
   * Add a string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addStringArrayProperty(String key, Supplier<String[]> getter, Consumer<String[]> setter);

  /**
   * Add a constant string array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstStringArray(String key, String[] value);

  /**
   * Add a read only raw property.
   *
   * @param key property name
   * @param typeString type string
   * @param getter getter function (returns current value)
   */
  default void addRawProperty(String key, String typeString, Supplier<byte[]> getter) {
    addRawProperty(key, typeString, getter, null);
  }

  /**
   * Add a raw property.
   *
   * @param key property name
   * @param typeString type string
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addRawProperty(
      String key, String typeString, Supplier<byte[]> getter, Consumer<byte[]> setter);

  /**
   * Add a constant raw property.
   *
   * @param key property name
   * @param typeString type string
   * @param value the value
   */
  void publishConstRaw(String key, String typeString, byte[] value);

  /**
   * Gets the kind of backend being used.
   *
   * @return Backend kind
   */
  BackendKind getBackendKind();

  /**
   * Return whether this sendable has been published.
   *
   * @return True if it has been published, false if not.
   */
  boolean isPublished();

  /** Update the published values by calling the getters for all properties. */
  void update();

  /** Clear properties. */
  void clearProperties();

  /**
   * Adds a closeable. The closeable.close() will be called when close() is called.
   *
   * @param closeable closeable object
   */
  void addCloseable(AutoCloseable closeable);
}
