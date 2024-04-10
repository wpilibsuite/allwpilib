// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.telemetry;

import edu.wpi.first.networktables.NetworkSendableTable;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.sendable2.Sendable;
import edu.wpi.first.util.sendable2.SendableOption;
import edu.wpi.first.util.sendable2.SendableTable;
import edu.wpi.first.util.struct.Struct;

import java.nio.ByteBuffer;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** Helper class for building Sendable dashboard representations. */
public class Telemetry {
  /** The {@link SendableTable} used by {@link Telemetry}. */
  private static SendableTable table;

  static {
    table = new NetworkSendableTable(NetworkTableInstance.getDefault(), "/telemetry");
  }

  private Telemetry() {
    throw new UnsupportedOperationException("This is a utility class!");
  }

  public static void setTable(SendableTable table) {
    Telemetry.table = table;
  }

  public static SendableTable getTable() {
    return table;
  }

  public static boolean getBoolean(String name, boolean defaultValue) {
    return getTable().getBoolean(name, defaultValue);
  }

  public static long getInteger(String name, long defaultValue) {
    return getTable().getInteger(name, defaultValue);
  }

  public static float getFloat(String name, float defaultValue) {
    return getTable().getFloat(name, defaultValue);
  }

  public static double getDouble(String name, double defaultValue) {
    return getTable().getDouble(name, defaultValue);
  }

  public static String getString(String name, String defaultValue) {
    return getTable().getString(name, defaultValue);
  }

  public static boolean[] getBooleanArray(String name, boolean[] defaultValue) {
    return getTable().getBooleanArray(name, defaultValue);
  }

  public static long[] getIntegerArray(String name, long[] defaultValue) {
    return getTable().getIntegerArray(name, defaultValue);
  }

  public static float[] getFloatArray(String name, float[] defaultValue) {
    return getTable().getFloatArray(name, defaultValue);
  }

  public static double[] getDoubleArray(String name, double[] defaultValue) {
    return getTable().getDoubleArray(name, defaultValue);
  }

  public static String[] getStringArray(String name, String[] defaultValue) {
    return getTable().getStringArray(name, defaultValue);
  }

  public static byte[] getRaw(String name, String typeString, byte[] defaultValue) {
    return getTable().getRaw(name, typeString, defaultValue);
  }

  public static <T> T getStruct(String name, Struct<T> struct, T defaultValue) {
    return getTable().getStruct(name, struct, defaultValue);
  }

  public static <T> boolean getStructInto(String name, T out, Struct<T> struct) {
    return getTable().getStructInto(name, out, struct);
  }

  public static <T> T getProtobuf(String name, Protobuf<T, ?> proto, T defaultValue) {
    return getTable().getProtobuf(name, proto, defaultValue);
  }

  public static <T> boolean getProtobufInto(String name, T out, Protobuf<T, ?> proto) {
    return getTable().getProtobufInto(name, out, proto);
  }

  public static void setBoolean(String name, boolean value) {
    getTable().setBoolean(name, value);
  }

  public static void setInteger(String name, long value) {
    getTable().setInteger(name, value);
  }

  public static void setFloat(String name, float value) {
    getTable().setFloat(name, value);
  }

  public static void setDouble(String name, double value) {
    getTable().setDouble(name, value);
  }

  public static void setString(String name, String value) {
    getTable().setString(name, value);
  }

  public static void setBooleanArray(String name, boolean[] value) {
    getTable().setBooleanArray(name, value);
  }

  public static void setIntegerArray(String name, long[] value) {
    getTable().setIntegerArray(name, value);
  }

  public static void setFloatArray(String name, float[] value) {
    getTable().setFloatArray(name, value);
  }

  public static void setDoubleArray(String name, double[] value) {
    getTable().setDoubleArray(name, value);
  }

  public static void setStringArray(String name, String[] value) {
    getTable().setStringArray(name, value);
  }

  public static void setRaw(String name, String typeString, byte[] value, int start, int len) {
    getTable().setRaw(name, typeString, value, start, len);
  }

  public static void setRaw(String name, String typeString, ByteBuffer value, int start, int len) {
    getTable().setRaw(name, typeString, value, start, len);
  }

  public static <T> void setStruct(String name, T value, Struct<T> struct) {
    getTable().setStruct(name, value, struct);
  }

  public static <T> void setProtobuf(String name, T value, Protobuf<T, ?> proto) {
    getTable().setProtobuf(name, value, proto);
  }

  public static void publishBoolean(String name, BooleanSupplier supplier) {
    getTable().publishBoolean(name, supplier);
  }

  public static void publishInteger(String name, LongSupplier supplier) {
    getTable().publishInteger(name, supplier);
  }

  public static void publishFloat(String name, FloatSupplier supplier) {
    getTable().publishFloat(name, supplier);
  }

  public static void publishDouble(String name, DoubleSupplier supplier) {
    getTable().publishDouble(name, supplier);
  }

  public static void publishString(String name, Supplier<String> supplier) {
    getTable().publishString(name, supplier);
  }

  public static void publishBooleanArray(String name, Supplier<boolean[]> supplier) {
    getTable().publishBooleanArray(name, supplier);
  }

  public static void publishIntegerArray(String name, Supplier<long[]> supplier) {
    getTable().publishIntegerArray(name, supplier);
  }

  public static void publishFloatArray(String name, Supplier<float[]> supplier) {
    getTable().publishFloatArray(name, supplier);
  }

  public static void publishDoubleArray(String name, Supplier<double[]> supplier) {
    getTable().publishDoubleArray(name, supplier);
  }

  public static void publishStringArray(String name, Supplier<String[]> supplier) {
    getTable().publishStringArray(name, supplier);
  }

  public static void publishRawBytes(String name, String typeString, Supplier<byte[]> supplier) {
    getTable().publishRawBytes(name, typeString, supplier);
  }

  public static void publishRawBuffer(String name, String typeString, Supplier<ByteBuffer> supplier) {
    getTable().publishRawBuffer(name, typeString, supplier);
  }

  public static <T> void publishStruct(String name, Struct<T> struct, Supplier<T> supplier) {
    getTable().publishStruct(name, struct, supplier);
  }

  public static <T> void publishProtobuf(String name, Protobuf<T, ?> proto, Supplier<T> supplier) {
    getTable().publishProtobuf(name, proto, supplier);
  }

  public static BooleanConsumer addBooleanPublisher(String name) {
    return getTable().addBooleanPublisher(name);
  }

  public static LongConsumer addIntegerPublisher(String name) {
    return getTable().addIntegerPublisher(name);
  }

  public static FloatConsumer addFloatPublisher(String name) {
    return getTable().addFloatPublisher(name);
  }

  public static DoubleConsumer addDoublePublisher(String name) {
    return getTable().addDoublePublisher(name);
  }

  public static Consumer<String> addStringPublisher(String name) {
    return getTable().addStringPublisher(name);
  }

  public static Consumer<boolean[]> addBooleanArrayPublisher(String name) {
    return getTable().addBooleanArrayPublisher(name);
  }

  public static Consumer<long[]> addIntegerArrayPublisher(String name) {
    return getTable().addIntegerArrayPublisher(name);
  }

  public static Consumer<float[]> addFloatArrayPublisher(String name) {
    return getTable().addFloatArrayPublisher(name);
  }

  public static Consumer<double[]> addDoubleArrayPublisher(String name) {
    return getTable().addDoubleArrayPublisher(name);
  }

  public static Consumer<String[]> addStringArrayPublisher(String name) {
    return getTable().addStringArrayPublisher(name);
  }

  public static Consumer<byte[]> addRawBytesPublisher(String name, String typeString) {
    return getTable().addRawBytesPublisher(name, typeString);
  }

  public static Consumer<ByteBuffer> addRawBufferPublisher(String name, String typeString) {
    return getTable().addRawBufferPublisher(name, typeString);
  }

  public static <T> Consumer<T> addStructPublisher(String name, Struct<T> struct) {
    return getTable().addStructPublisher(name, struct);
  }

  public static <T> Consumer<T> addProtobufPublisher(String name, Protobuf<T, ?> proto) {
    return getTable().addProtobufPublisher(name, proto);
  }

  public static void subscribeBoolean(String name, BooleanConsumer consumer) {
    getTable().subscribeBoolean(name, consumer);
  }

  public static void subscribeInteger(String name, LongConsumer consumer) {
    getTable().subscribeInteger(name, consumer);
  }

  public static void subscribeFloat(String name, FloatConsumer consumer) {
    getTable().subscribeFloat(name, consumer);
  }

  public static void subscribeDouble(String name, DoubleConsumer consumer) {
    getTable().subscribeDouble(name, consumer);
  }

  public static void subscribeString(String name, Consumer<String> consumer) {
    getTable().subscribeString(name, consumer);
  }

  public static void subscribeBooleanArray(String name, Consumer<boolean[]> consumer) {
    getTable().subscribeBooleanArray(name, consumer);
  }

  public static void subscribeIntegerArray(String name, Consumer<long[]> consumer) {
    getTable().subscribeIntegerArray(name, consumer);
  }

  public static void subscribeFloatArray(String name, Consumer<float[]> consumer) {
    getTable().subscribeFloatArray(name, consumer);
  }

  public static void subscribeDoubleArray(String name, Consumer<double[]> consumer) {
    getTable().subscribeDoubleArray(name, consumer);
  }

  public static void subscribeStringArray(String name, Consumer<String[]> consumer) {
    getTable().subscribeStringArray(name, consumer);
  }

  public static void subscribeRawBytes(String name, String typeString, Consumer<byte[]> consumer) {
    getTable().subscribeRawBytes(name, typeString, consumer);
  }

  public static <T> void subscribeStruct(String name, Struct<T> struct, Consumer<T> consumer) {
    getTable().subscribeStruct(name, struct, consumer);
  }

  public static <T> void subscribeProtobuf(String name, Protobuf<T, ?> proto, Consumer<T> consumer) {
    getTable().subscribeProtobuf(name, proto, consumer);
  }

  public static <T> SendableTable addSendable(String name, T obj, Sendable<T> sendable) {
    return getTable().addSendable(name, obj, sendable);
  }

  public static SendableTable getChild(String name) {
    return getTable().getChild(name);
  }

  public static void setPublishOptions(String name, SendableOption... options) {
    getTable().setPublishOptions(name, options);
  }

  public static void setSubscribeOptions(String name, SendableOption... options) {
    getTable().setSubscribeOptions(name, options);
  }

  /**
   * Gets the current value of a property (as a JSON string).
   *
   * @param name name
   * @param propName property name
   * @return JSON string; "null" if the property does not exist.
   */
  public static String getProperty(String name, String propName) {
    return getTable().getProperty(name, propName);
  }

  /**
   * Sets a property value.
   *
   * @param name name
   * @param propName property name
   * @param value property value (JSON string)
   * @throws IllegalArgumentException if properties is not parseable as JSON
   */
  public static void setProperty(String name, String propName, String value) {
    getTable().setProperty(name, propName, value);
  }

  /**
   * Deletes a property. Has no effect if the property does not exist.
   *
   * @param name name
   * @param propName property name
   */
  public static void deleteProperty(String name, String propName) {
    getTable().deleteProperty(name, propName);
  }

  /**
   * Gets all topic properties as a JSON object string. Each key in the object is the property name,
   * and the corresponding value is the property value.
   *
   * @param name name
   * @return JSON string
   */
  public static String getProperties(String name) {
    return getTable().getProperties(name);
  }

  /**
   * Updates multiple topic properties. Each key in the passed-in object is the name of the property
   * to add/update, and the corresponding value is the property value to set for that property. Null
   * values result in deletion of the corresponding property.
   *
   * @param name name
   * @param properties JSON object string with keys to add/update/delete
   * @throws IllegalArgumentException if properties is not a JSON object
   */
  public static void setProperties(String name, String properties) {
    getTable().setProperties(name, properties);
  }

  public static void remove(String name) {
    getTable().remove(name);
  }

  /** Erases all publishers and subscribers. */
  public static void clear() {
    getTable().clear();
  }
}
