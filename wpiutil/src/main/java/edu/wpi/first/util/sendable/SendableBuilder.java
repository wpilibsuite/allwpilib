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
import java.util.function.IntSupplier;
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
   * Set a flag indicating if this Sendable should be treated as an actuator. By default, this flag
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
   * Add a constant boolean property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstBoolean(String key, boolean value);

  /**
   * Add an integer property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addIntegerProperty(String key, LongSupplier getter, LongConsumer setter);

  /**
   * Add a constant integer property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstInteger(String key, long value);

  /**
   * Add a float property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addFloatProperty(String key, FloatSupplier getter, FloatConsumer setter);

  /**
   * Add a constant float property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstFloat(String key, float value);

  /**
   * Add a double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addDoubleProperty(String key, DoubleSupplier getter, DoubleConsumer setter);

  /**
   * Add a constant double property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstDouble(String key, double value);

  /**
   * Add a string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addStringProperty(String key, Supplier<String> getter, Consumer<String> setter);

  /**
   * Add a constant string property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstString(String key, String value);

  /**
   * Add a boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addBooleanArrayProperty(String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter);

  /**
   * Add a constant boolean array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstBooleanArray(String key, boolean[] value);

  /**
   * Add an integer array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addIntegerArrayProperty(String key, Supplier<long[]> getter, Consumer<long[]> setter);

  /**
   * Add a constant integer property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstIntegerArray(String key, long[] value);

  /**
   * Add a float array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addFloatArrayProperty(String key, Supplier<float[]> getter, Consumer<float[]> setter);

  /**
   * Add a constant float array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstFloatArray(String key, float[] value);

  /**
   * Add a double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addDoubleArrayProperty(String key, Supplier<double[]> getter, Consumer<double[]> setter);

  /**
   * Add a constant double array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstDoubleArray(String key, double[] value);

  /**
   * Add a string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  void addStringArrayProperty(String key, Supplier<String[]> getter, Consumer<String[]> setter);

  /**
   * Add a constant string array property.
   *
   * @param key property name
   * @param value the value
   */
  void publishConstStringArray(String key, String[] value);

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
   * Add a constant raw property.
   *
   * @param key property name
   * @param typeString type string
   * @param value the value
   */
  void publishConstRaw(String key, String typeString, byte[] value);

  /**
   * Functional interface for adding a generic object-based property. This is expected to be used
   * via a method reference to a SendableBuilder instance in a Sendable implementation's {@code
   * initSendable} method in conjunction with {@link #caching the builder's caching function}.
   *
   * <p>
   *
   * <pre>
   *  {@literal @}Override
   *   public void initSendable(SendableBuilder builder) {
   *     builder.caching(
   *       "Things",
   *       () -> m_things.hashCode(),
   *       () -> m_things.stream().map(Thing::getName).toArray(String[]::new),
   *       null,
   *       builder::addStringArrayProperty
   *     );
   *   }
   * </pre>
   *
   * @param <T> the type of the values in the property
   */
  @FunctionalInterface
  interface PropertyFn<T> {
    /**
     * Adds a property.
     *
     * @param key property name
     * @param getter getter function (returns current value)
     * @param setter setter function (sets new value)
     */
    void addProperty(String key, Supplier<T> getter, Consumer<T> setter);
  }

  /**
   * Wraps a property with a caching function. Values will be read from a cache, which will only be
   * updated when the cache key changes. This is useful for avoiding expensive recomputation of
   * properties when it is possible to know ahead of time if the property value will change. This
   * function can only be applied to string and array-type properties.
   *
   * <p>
   *
   * <pre>
   *   private List&lt;Thing&gt; m_list;
   *
   *  {@literal @}Override
   *   public void initSendable(SendableBuilder builder) {
   *     builder.caching(
   *       "Thing Names",
   *       m_list::hashCode,
   *       () -> m_list.stream().map(Thing::getName).toArray(String[]::new),
   *       null,
   *       builder::addStringArrayProperty
   *     )
   *   }
   * </pre>
   *
   * @param key property name
   * @param hashFunction the hash function to use to determine when the cache needs to be updated
   * @param getter getter function (returns current value). Only called when the cache is updated
   * @param setter setter function (sets new value). Unaffected by caching, and will be called as
   *     normal
   * @param property the builder property function to wrap
   * @param <T> the type of the values of the property
   */
  <T> void caching(
      String key,
      IntSupplier hashFunction,
      Supplier<T> getter,
      Consumer<T> setter,
      PropertyFn<T> property);

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
