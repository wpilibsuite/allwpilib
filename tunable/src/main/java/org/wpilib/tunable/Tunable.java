// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.function.Consumer;
import java.util.function.Supplier;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/**
 * A tunable value.
 *
 * @param <T> value class
 */
public interface Tunable<T> extends Supplier<T>, Consumer<T> {
  static class StructTunable<T> implements Tunable<T> {
    StructTunable(T initialValue, Struct<T> struct) {
      m_struct = struct;
      m_value = initialValue;
    }

    @Override
    public void set(T value) {
      m_value = value;
    }

    @Override
    public T get() {
      return m_value;
    }

    public Struct<T> getStruct() {
      return m_struct;
    }

    private final Struct<T> m_struct;
    private T m_value;
  }

  static class ProtobufTunable<T> implements Tunable<T> {
    ProtobufTunable(T initialValue, Protobuf<T, ?> proto) {
      m_proto = proto;
      m_value = initialValue;
    }

    @Override
    public void set(T value) {
      m_value = value;
    }

    @Override
    public T get() {
      return m_value;
    }

    public Protobuf<T, ?> getProtobuf() {
      return m_proto;
    }

    private final Protobuf<T, ?> m_proto;
    private T m_value;
  }

  static <T> Tunable<T> create() {
    return create(null);
  }

  static <T> Tunable<T> create(T initialValue) {
    return new Tunable<T>() {
      @Override
      public void set(T value) {
        m_value = value;
      }

      @Override
      public T get() {
        return m_value;
      }

      private T m_value = initialValue;
    };
  }

  /**
   * Creates a tunable object with a specific Struct serializer.
   *
   * @param <T> data type
   * @param initialValue the initial value (may be null)
   * @param struct struct serializer
   * @return Tunable
   */
  static <T> Tunable<T> create(T initialValue, Struct<T> struct) {
    return new StructTunable<T>(initialValue, struct);
  }

  /**
   * Creates a tunable object with a specific Protobuf serializer.
   *
   * @param <T> data type
   * @param initialValue the initial value (may be null)
   * @param proto protobuf serializer
   * @return Tunable
   */
  static <T> Tunable<T> create(T initialValue, Protobuf<T, ?> proto) {
    return new ProtobufTunable<T>(initialValue, proto);
  }

  static <T> Tunable<T> create(Supplier<T> getter, Consumer<T> setter) {
    return new Tunable<T>() {
      @Override
      public void set(T value) {
        setter.accept(value);
      }

      @Override
      public T get() {
        return getter.get();
      }
    };
  }

  /**
   * Sets the value.
   *
   * @param value value
   */
  void set(T value);

  /**
   * Gets the value.
   *
   * @return value
   */
  @Override
  T get();

  @Override
  default void accept(T value) {
    set(value);
  }
}
