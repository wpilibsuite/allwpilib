// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import edu.wpi.first.units.Measure;
import edu.wpi.first.units.Unit;
import edu.wpi.first.util.struct.Struct;
import java.util.Collection;

/** A backend is a generic interface for Epilogue to log discrete data points. */
public interface EpilogueBackend {
  /**
   * Creates a backend that logs to multiple backends at once. Data reads will still only occur
   * once; data is passed to all composed backends at once.
   *
   * @param backends the backends to compose together
   * @return the multi backend
   */
  static EpilogueBackend multi(EpilogueBackend... backends) {
    return new MultiBackend(backends);
  }

  /**
   * Creates a lazy version of this backend. A lazy backend will only log data to a field when its
   * value changes, which can help keep file size and bandwidth usage in check. However, there is an
   * additional CPU and memory overhead associated with tracking the current value of every logged
   * entry. The most surefire way to reduce CPU and memory usage associated with logging is to log
   * fewer things - which can be done by opting out of logging unnecessary data or increasing the
   * minimum logged importance level in the Epilogue configuration.
   *
   * @return the lazy backend
   */
  default EpilogueBackend lazy() {
    return new LazyBackend(this);
  }

  /**
   * Gets a backend that can be used to log nested data underneath a specific path.
   *
   * @param path the path to use for logging nested data under
   * @return the nested backend
   */
  EpilogueBackend getNested(String path);

  /**
   * Logs a 32-bit integer data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, int value);

  /**
   * Logs a 64-bit integer data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, long value);

  /**
   * Logs a 32-bit floating point data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, float value);

  /**
   * Logs a 64-bit floating point data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, double value);

  /**
   * Logs a boolean data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, boolean value);

  /**
   * Logs a raw byte array data point. <strong>NOTE:</strong> serializable data should be logged
   * using {@link #log(String, Object, Struct)}.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, byte[] value);

  /**
   * Logs a 32-bit integer array data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, int[] value);

  /**
   * Logs a 64-bit integer array data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, long[] value);

  /**
   * Logs a 32-bit floating point array data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, float[] value);

  /**
   * Logs a 64-bit floating point array data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, double[] value);

  /**
   * Logs a boolean array data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, boolean[] value);

  /**
   * Logs a text data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, String value);

  /**
   * Logs a string array data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  void log(String identifier, String[] value);

  /**
   * Logs a collection of strings data point.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   */
  default void log(String identifier, Collection<String> value) {
    log(identifier, value.toArray(String[]::new));
  }

  /**
   * Logs a struct-serializable object.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   * @param struct the struct to use to serialize the data
   * @param <S> the serializable type
   */
  <S> void log(String identifier, S value, Struct<S> struct);

  /**
   * Logs an array of struct-serializable objects.
   *
   * @param identifier the identifier of the data point
   * @param value the value of the data point
   * @param struct the struct to use to serialize the objects
   * @param <S> the serializable type
   */
  <S> void log(String identifier, S[] value, Struct<S> struct);

  /**
   * Logs a collection of struct-serializable objects.
   *
   * @param identifier the identifier of the data
   * @param value the collection of objects to log
   * @param struct the struct to use to serialize the objects
   * @param <S> the serializable type
   */
  default <S> void log(String identifier, Collection<S> value, Struct<S> struct) {
    @SuppressWarnings("unchecked")
    S[] array = (S[]) value.toArray(Object[]::new);
    log(identifier, array, struct);
  }

  /**
   * Logs a measurement's value in terms of its base unit.
   *
   * @param identifier the identifier of the data field
   * @param value the new value of the data field
   */
  default void log(String identifier, Measure<?> value) {
    log(identifier, value.baseUnitMagnitude());
  }

  /**
   * Logs a measurement's value in terms of another unit.
   *
   * @param identifier the identifier of the data field
   * @param value the new value of the data field
   * @param unit the unit to log the measurement in
   * @param <U> the dimension of the unit
   */
  default <U extends Unit> void log(String identifier, Measure<U> value, U unit) {
    log(identifier, value.in(unit));
  }

  /**
   * Logs an enum value. The value will appear as a string entry using the name of the enum.
   *
   * @param identifier the identifier of the data field
   * @param value the new value of the data field
   */
  default void log(String identifier, Enum<?> value) {
    log(identifier, value.name());
  }

  // TODO: Add default methods to support common no-struct no-sendable types like joysticks?
}
