// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

import java.lang.reflect.Array;
import java.nio.ByteBuffer;

/**
 * Interface for raw struct serialization.
 *
 * <p>This is designed for serializing small fixed-size data structures in the fastest and most
 * compact means possible. Serialization consists of making relative put() calls to a ByteBuffer and
 * deserialization consists of making relative get() calls from a ByteBuffer.
 *
 * <p>Idiomatically, classes that support raw struct serialization should provide a static final
 * member named "struct" that provides an instance of an implementation of this interface.
 *
 * @param <T> object type
 */
public interface Struct<T> {
  /** Serialized size of a "bool" value. */
  int kSizeBool = 1;

  /** Serialized size of an "int8" or "uint8" value. */
  int kSizeInt8 = 1;

  /** Serialized size of an "int16" or "uint16" value. */
  int kSizeInt16 = 2;

  /** Serialized size of an "int32" or "uint32" value. */
  int kSizeInt32 = 4;

  /** Serialized size of an "int64" or "uint64" value. */
  int kSizeInt64 = 8;

  /** Serialized size of an "float" or "float32" value. */
  int kSizeFloat = 4;

  /** Serialized size of an "double" or "float64" value. */
  int kSizeDouble = 8;

  /**
   * Gets the Class object for the stored value.
   *
   * @return Class
   */
  Class<T> getTypeClass();

  /**
   * Gets the type name (e.g. for schemas of other structs). This should be globally unique among
   * structs.
   *
   * @return type name
   */
  String getTypeName();

  /**
   * Gets the type string (e.g. for NetworkTables). This should be globally unique and start with
   * "struct:".
   *
   * @return type string
   */
  default String getTypeString() {
    return "struct:" + getTypeName();
  }

  /**
   * Gets the serialized size (in bytes). This should always be a constant.
   *
   * @return serialized size
   */
  int getSize();

  /**
   * Gets the schema.
   *
   * @return schema
   */
  String getSchema();

  /**
   * Gets the list of struct types referenced by this struct.
   *
   * @return list of struct types
   */
  default Struct<?>[] getNested() {
    return new Struct<?>[] {};
  }

  /**
   * Deserializes an object from a raw struct serialized ByteBuffer starting at the current
   * position. Will increment the ByteBuffer position by getStructSize() bytes. Will not otherwise
   * modify the ByteBuffer (e.g. byte order will not be changed).
   *
   * @param bb ByteBuffer
   * @return New object
   */
  T unpack(ByteBuffer bb);

  /**
   * Puts object contents to a ByteBuffer starting at the current position. Will increment the
   * ByteBuffer position by getStructSize() bytes. Will not otherwise modify the ByteBuffer (e.g.
   * byte order will not be changed).
   *
   * @param bb ByteBuffer
   * @param value object to serialize
   */
  void pack(ByteBuffer bb, T value);

  /**
   * Updates object contents from a raw struct serialized ByteBuffer starting at the current
   * position. Will increment the ByteBuffer position by getStructSize() bytes. Will not otherwise
   * modify the ByteBuffer (e.g. byte order will not be changed).
   *
   * <p>Immutable classes cannot and should not implement this function. The default implementation
   * throws UnsupportedOperationException.
   *
   * @param out object to update
   * @param bb ByteBuffer
   * @throws UnsupportedOperationException if the object is immutable
   */
  default void unpackInto(T out, ByteBuffer bb) {
    throw new UnsupportedOperationException("object does not support unpackInto");
  }

  /**
   * Deserializes an array from a raw struct serialized ByteBuffer starting at the current position.
   * Will increment the ByteBuffer position by size * struct.size() bytes. Will not otherwise modify
   * the ByteBuffer (e.g. byte order will not be changed).
   *
   * @param <T> Object type
   * @param bb ByteBuffer
   * @param size Size of the array
   * @param struct Struct implementation
   * @return Deserialized array
   */
  static <T> T[] unpackArray(ByteBuffer bb, int size, Struct<T> struct) {
    @SuppressWarnings("unchecked")
    T[] arr = (T[]) Array.newInstance(struct.getTypeClass(), size);
    for (int i = 0; i < arr.length; i++) {
      arr[i] = struct.unpack(bb);
    }
    return arr;
  }

  /**
   * Deserializes a double array from a raw struct serialized ByteBuffer starting at the current
   * position. Will increment the ByteBuffer position by size * kSizeDouble bytes. Will not
   * otherwise modify the ByteBuffer (e.g. byte order will not be changed).
   *
   * @param bb ByteBuffer
   * @param size Size of the array
   * @return Double array
   */
  static double[] unpackDoubleArray(ByteBuffer bb, int size) {
    double[] arr = new double[size];
    for (int i = 0; i < size; i++) {
      arr[i] = bb.getDouble();
    }
    return arr;
  }

  /**
   * Puts array contents to a ByteBuffer starting at the current position. Will increment the
   * ByteBuffer position by size * struct.size() bytes. Will not otherwise modify the ByteBuffer
   * (e.g. byte order will not be changed).
   *
   * @param <T> Object type
   * @param bb ByteBuffer
   * @param arr Array to serialize
   * @param struct Struct implementation
   */
  static <T> void packArray(ByteBuffer bb, T[] arr, Struct<T> struct) {
    for (T obj : arr) {
      struct.pack(bb, obj);
    }
  }

  /**
   * Puts array contents to a ByteBuffer starting at the current position. Will increment the
   * ByteBuffer position by size * kSizeDouble bytes. Will not otherwise modify the ByteBuffer (e.g.
   * byte order will not be changed).
   *
   * @param bb ByteBuffer
   * @param arr Array to serialize
   */
  static void packArray(ByteBuffer bb, double[] arr) {
    for (double obj : arr) {
      bb.putDouble(obj);
    }
  }

  /**
   * Returns whether or not objects are immutable. Immutable objects must also be comparable using
   * the equals() method. Default implementation returns false.
   *
   * @return True if object is immutable
   */
  default boolean isImmutable() {
    return false;
  }

  /**
   * Returns whether or not objects are cloneable using the clone() method. Cloneable objects must
   * also be comparable using the equals() method. Default implementation returns false.
   *
   * @return True if object is cloneable
   */
  default boolean isCloneable() {
    return false;
  }

  /**
   * Creates a (deep) clone of the object. May also return the object directly if the object is
   * immutable. Default implementation throws CloneNotSupportedException. Typically this should be
   * implemented by implementing clone() on the object itself, and calling it from here.
   *
   * @param obj object to clone
   * @return Clone of object (if immutable, may be same object)
   * @throws CloneNotSupportedException if clone not supported
   */
  default T clone(T obj) throws CloneNotSupportedException {
    throw new CloneNotSupportedException();
  }
}
