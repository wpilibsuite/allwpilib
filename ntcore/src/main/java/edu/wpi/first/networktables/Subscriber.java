// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables subscriber. */
public interface Subscriber extends PubSub {
  /**
   * Determines if the entry currently exists.
   *
   * @return True if the entry exists, false otherwise.
   */
  boolean exists();

  /**
   * Gets the last time the entry's value was changed.
   *
   * @return Entry last change time
   */
  long getLastChange();
}
