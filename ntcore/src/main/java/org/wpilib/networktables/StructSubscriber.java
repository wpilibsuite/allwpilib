// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import java.util.function.Supplier;

/**
 * NetworkTables struct-encoded value subscriber.
 *
 * @param <T> value class
 */
@SuppressWarnings("PMD.MissingOverride")
public interface StructSubscriber<T> extends Subscriber, Supplier<T> {
  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  @Override
  StructTopic<T> getTopic();

  /**
   * Get the last published value. If no value has been published or the value cannot be unpacked,
   * returns the stored default value.
   *
   * @return value
   */
  T get();

  /**
   * Get the last published value. If no value has been published or the value cannot be unpacked,
   * returns the passed defaultValue.
   *
   * @param defaultValue default value to return if no value has been published
   * @return value
   */
  T get(T defaultValue);

  /**
   * Get the last published value, replacing the contents in place of an existing object. If no
   * value has been published or the value cannot be unpacked, does not replace the contents and
   * returns false. This function will not work (will throw UnsupportedOperationException) unless T
   * is mutable (and the implementation of Struct implements unpackInto).
   *
   * <p>Note: due to Java language limitations, it's not possible to validate at compile time that
   * the out parameter is mutable.
   *
   * @param out object to replace contents of; must be mutable
   * @return true if successful, false if no value has been published
   * @throws UnsupportedOperationException if T is immutable
   */
  boolean getInto(T out);

  /**
   * Get the last published value along with its timestamp. If no value has been published or the
   * value cannot be unpacked, returns the stored default value and a timestamp of 0.
   *
   * @return timestamped value
   */
  TimestampedObject<T> getAtomic();

  /**
   * Get the last published value along with its timestamp If no value has been published or the
   * value cannot be unpacked, returns the passed defaultValue and a timestamp of 0.
   *
   * @param defaultValue default value to return if no value has been published
   * @return timestamped value
   */
  TimestampedObject<T> getAtomic(T defaultValue);

  /**
   * Get an array of all valid value changes since the last call to readQueue. Also provides a
   * timestamp for each value. Values that cannot be unpacked are dropped.
   *
   * <p>The "poll storage" subscribe option can be used to set the queue depth.
   *
   * @return Array of timestamped values; empty array if no valid new changes have been published
   *     since the previous call.
   */
  TimestampedObject<T>[] readQueue();

  /**
   * Get an array of all value changes since the last call to readQueue. Values that cannot be
   * unpacked are dropped.
   *
   * <p>The "poll storage" subscribe option can be used to set the queue depth.
   *
   * @return Array of values; empty array if no valid new changes have been published since the
   *     previous call.
   */
  T[] readQueueValues();
}
