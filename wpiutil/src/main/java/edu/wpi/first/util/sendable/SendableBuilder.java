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

@SuppressWarnings("UnusedReturnValue")
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
   * Set a flag indicating if this Sendable should be treated as an actuator. By default, this flag
   * is false.
   *
   * @param value true if actuator, false if not
   */
  void setActuator(boolean value);

  /**
   * Set a flag indicating if this Sendable should be controllable from the dashboard. By default,
   * this flag is false.
   *
   * @param controllable true if controllable, false if not
   */
  void setControllable(boolean controllable);

  /**
   * Set the function that should be called to set the Sendable into a safe state. This is called
   * when entering and exiting Live Window mode.
   *
   * @param func function
   */
  void setSafeState(Runnable func);

  void selfMetadata(SendableMetadata metadata);

  /**
   * Add a boolean property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addBooleanProperty(
      String key, BooleanSupplier getter, BooleanConsumer setter);

  /**
   * Add an integer property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addIntegerProperty(String key, LongSupplier getter, LongConsumer setter);

  /**
   * Add a float property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addFloatProperty(String key, FloatSupplier getter, FloatConsumer setter);

  /**
   * Add a double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addDoubleProperty(
      String key, DoubleSupplier getter, DoubleConsumer setter);

  /**
   * Add a string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addStringProperty(
      String key, Supplier<String> getter, Consumer<String> setter);

  /**
   * Add a boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addBooleanArrayProperty(
      String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter);

  /**
   * Add an integer array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addIntegerArrayProperty(
      String key, Supplier<long[]> getter, Consumer<long[]> setter);

  /**
   * Add a float array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addFloatArrayProperty(
      String key, Supplier<float[]> getter, Consumer<float[]> setter);

  /**
   * Add a double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addDoubleArrayProperty(
      String key, Supplier<double[]> getter, Consumer<double[]> setter);

  /**
   * Add a string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addStringArrayProperty(
      String key, Supplier<String[]> getter, Consumer<String[]> setter);

  /**
   * Add a raw property.
   *
   * @param key property name
   * @param typeString type string
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   * @return property config object
   */
  SendablePropertyBuilder addRawProperty(
      String key, String typeString, Supplier<byte[]> getter, Consumer<byte[]> setter);

  /**
   * Add a Sendable property as a nested child.
   *
   * @param key property name
   * @param sendable child
   * @return property config object
   */
  SendablePropertyBuilder addSendable(String key, Sendable sendable);

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
