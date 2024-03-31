// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.sendable2;

import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.struct.Struct;

import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** Helper class for building Sendable dashboard representations. */
public interface SendableTable extends AutoCloseable {
  public static class PubSubOption {
    // TODO: Move ntcore PubSubOption to wpiutil?
  }

  void setBoolean(String name, boolean value);

  void publishBoolean(String name, BooleanSupplier supplier, PubSubOption... options);

  BooleanConsumer publishBoolean(String name, PubSubOption... options);

  void subscribeBoolean(String name, BooleanConsumer consumer, PubSubOption... options);

  void setInt(String name, long value);

  void publishInt(String name, LongSupplier supplier, PubSubOption... options);

  LongConsumer publishInt(String name, PubSubOption... options);

  void subscribeInt(String name, LongConsumer consumer, PubSubOption... options);

  void setFloat(String name, float value);

  void publishFloat(String name, FloatSupplier supplier, PubSubOption... options);

  FloatConsumer publishFloat(String name, PubSubOption... options);

  void subscribeFloat(String name, FloatConsumer consumer, PubSubOption... options);

  void setDouble(String name, double value);

  void publishDouble(String name, DoubleSupplier supplier, PubSubOption... options);

  DoubleConsumer publishDouble(String name, PubSubOption... options);

  void subscribeDouble(String name, DoubleConsumer consumer, PubSubOption... options);

  void setString(String name, String value);

  void publishString(String name, Supplier<String> supplier, PubSubOption... options);

  Consumer<String> publishString(String name, PubSubOption... options);

  void subscribeString(String name, Consumer<String> consumer, PubSubOption... options);

  <T> void setStruct(String name, T value, Struct<T> struct);

  <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier, PubSubOption... options);

  <T> Consumer<T> publishStruct(String name, Struct<T> struct, PubSubOption... options);

  <T> void subscribeStruct(String name, Struct<T> struct, Consumer<T> consumer, PubSubOption... options);

  <T> T addSendable(String name, T obj, Sendable<T> sendable);

  <T> T getSendable(String name);

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
   * Return whether close() has been called on this sendable table.
   *
   * @return True if closed, false otherwise.
   */
  boolean isClosed();
}