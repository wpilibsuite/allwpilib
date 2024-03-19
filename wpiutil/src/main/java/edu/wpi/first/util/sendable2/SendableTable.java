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
  void setBoolean(String name, boolean value);

  void publishBoolean(String name, BooleanSupplier getter);  // TODO: options

  void subscribeBoolean(String name, BooleanConsumer getter);  // TODO: options

  void setDouble(String name, double value);

  void publishDouble(String name, DoubleSupplier getter);  // TODO: options

  void subscribeDouble(String name, DoubleConsumer getter);  // TODO: options

  void setFloat(String name, float value);

  void publishFloat(String name, FloatSupplier getter);  // TODO: options

  void subscribeFloat(String name, FloatConsumer getter);  // TODO: options

  void setInt(String name, long value);

  void publishInt(String name, LongSupplier getter);  // TODO: options

  void subscribeInt(String name, LongConsumer getter);  // TODO: options

  void setString(String name, String value);

  void publishString(String name, Supplier<String> getter);  // TODO: options

  void subscribeString(String name, Consumer<String> getter);  // TODO: options

  <T> void setStruct(String name, T value, Struct<T> struct);

  <T> void publishStruct(String name, Struct<T> struct, Supplier<T> getter);  // TODO: options

  <T> void subscribeStruct(String name, Struct<T> struct, Consumer<T> getter);  // TODO: options

  <T> T addSendable(String name, T obj, Sendable<T> sendable);

  <T> T getSendable(String name);

  void erase(String name);

  /*
  public interface BooleanNode {
    void set(boolean value);
    boolean get();
    void publish(BooleanSupplier getter);  // TODO: options
    void subscribe(BooleanConsumer setter);  // TODO: options
  }

  BooleanNode booleanNode(String name);

  public interface StructNode<T> {
    void set(T value);
    T get();
    void publish(Supplier<T> getter);  // TODO: options
    void subscribe(Consumer<T> setter);  // TODO: options
  }

  <T> StructNode<T> structNode(String name, Struct<T> struct);

  public interface SendableNode<T> {
    void init(T obj);
    void close(T obj);
  }

  <T> SendableNode<T> sendableNode(String name, Sendable<T> sendable);
  */

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
}
