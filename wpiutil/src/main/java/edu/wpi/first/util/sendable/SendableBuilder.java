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

public interface SendableBuilder extends AutoCloseable {
  /** The backend kinds used for the sendable builder. */
  enum BackendKind {
    kUnknown,
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
   * Set a flag indicating if this sendable should be treated as an actuator. By default this flag
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
   * Add a boolean property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addBooleanProperty(String key, BooleanSupplier getter, BooleanConsumer setter);

  /**
   * Add an integer property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addIntegerProperty(String key, LongSupplier getter, LongConsumer setter);

  /**
   * Add a float property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addFloatProperty(String key, FloatSupplier getter, FloatConsumer setter);

  /**
   * Add a double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addDoubleProperty(String key, DoubleSupplier getter, DoubleConsumer setter);

  /**
   * Add a string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addStringProperty(String key, Supplier<String> getter, Consumer<String> setter);

  /**
   * Add a boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addBooleanArrayProperty(String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter);

  /**
   * Add an integer array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addIntegerArrayProperty(String key, Supplier<long[]> getter, Consumer<long[]> setter);

  /**
   * Add a float array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addFloatArrayProperty(String key, Supplier<float[]> getter, Consumer<float[]> setter);

  /**
   * Add a double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addDoubleArrayProperty(String key, Supplier<double[]> getter, Consumer<double[]> setter);

  /**
   * Add a string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addStringArrayProperty(String key, Supplier<String[]> getter, Consumer<String[]> setter);

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
