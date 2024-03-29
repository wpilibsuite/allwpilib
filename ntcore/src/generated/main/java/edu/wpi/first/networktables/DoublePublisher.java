// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// THIS FILE WAS AUTO-GENERATED BY ./ntcore/generate_topics.py. DO NOT MODIFY

package edu.wpi.first.networktables;

import java.util.function.DoubleConsumer;

/** NetworkTables Double publisher. */
public interface DoublePublisher extends Publisher, DoubleConsumer {
  /**
   * Get the corresponding topic.
   *
   * @return Topic
   */
  @Override
  DoubleTopic getTopic();

  /**
   * Publish a new value using current NT time.
   *
   * @param value value to publish
   */
  default void set(double value) {
    set(value, 0);
  }


  /**
   * Publish a new value.
   *
   * @param value value to publish
   * @param time timestamp; 0 indicates current NT time should be used
   */
  void set(double value, long time);

  /**
   * Publish a default value.
   * On reconnect, a default value will never be used in preference to a
   * published value.
   *
   * @param value value
   */
  void setDefault(double value);

  @Override
  default void accept(double value) {
    set(value);
  }
}
