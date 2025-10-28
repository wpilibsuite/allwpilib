// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import java.util.function.Consumer;

/**
 * NetworkTables protobuf-encoded value publisher.
 *
 * @param <T> value class
 */
public interface ProtobufPublisher<T> extends Publisher, Consumer<T> {
  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  @Override
  ProtobufTopic<T> getTopic();

  /**
   * Publish a new value using current NT time.
   *
   * @param value value to publish
   */
  default void set(T value) {
    set(value, 0);
  }

  /**
   * Publish a new value.
   *
   * @param value value to publish
   * @param time timestamp; 0 indicates current NT time should be used
   */
  void set(T value, long time);

  /**
   * Publish a default value. On reconnect, a default value will never be used in preference to a
   * published value.
   *
   * @param value value
   */
  void setDefault(T value);

  @Override
  default void accept(T value) {
    set(value);
  }
}
