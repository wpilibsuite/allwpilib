// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables publisher or subscriber. */
public interface PubSub extends AutoCloseable {
  @Override
  void close();

  /**
   * Gets the subscribed-to / published-to topic.
   *
   * @return Topic
   */
  Topic getTopic();

  /**
   * Determines if the native handle is valid.
   *
   * @return True if the native handle is valid, false otherwise.
   */
  boolean isValid();

  /**
   * Gets the native handle.
   *
   * @return Handle
   */
  int getHandle();
}
