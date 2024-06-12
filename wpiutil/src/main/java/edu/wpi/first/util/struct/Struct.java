// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.struct;

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
   * Gets the type string (e.g. for NetworkTables). This should be globally unique and start with
   * "struct:".
   *
   * @return type string
   */
  String getTypeString();

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
}
