// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable;

import edu.wpi.first.util.function.BooleanConsumer;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.Supplier;

public interface SendableBuilder {
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
  SendableBuilder setSmartDashboardType(String type);

  /**
   * Set a flag indicating if this sendable should be treated as an actuator. By default this flag
   * is false.
   *
   * @param value true if actuator, false if not
   */
  SendableBuilder setActuator(boolean value);

  /**
   * Set the function that should be called to set the Sendable into a safe state. This is called
   * when entering and exiting Live Window mode.
   *
   * @param func function
   */
  SendableBuilder setSafeState(Runnable func);

  /**
   * Add a boolean property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  SendableBuilder addBooleanProperty(String key, BooleanSupplier getter, BooleanConsumer setter);

  /**
   * Add a double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  SendableBuilder addDoubleProperty(String key, DoubleSupplier getter, DoubleConsumer setter);

  /**
   * Add a string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  SendableBuilder addStringProperty(String key, Supplier<String> getter, Consumer<String> setter);

  /**
   * Add a boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  SendableBuilder addBooleanArrayProperty(
      String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter);

  /**
   * Add a double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  SendableBuilder addDoubleArrayProperty(
      String key, Supplier<double[]> getter, Consumer<double[]> setter);

  /**
   * Add a string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  SendableBuilder addStringArrayProperty(
      String key, Supplier<String[]> getter, Consumer<String[]> setter);

  /**
   * Add a raw property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  SendableBuilder addRawProperty(String key, Supplier<byte[]> getter, Consumer<byte[]> setter);

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
}
