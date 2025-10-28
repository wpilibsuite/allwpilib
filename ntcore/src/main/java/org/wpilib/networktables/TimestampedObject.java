// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

/**
 * NetworkTables timestamped object.
 *
 * @param <T> Value type.
 */
public final class TimestampedObject<T> {
  /**
   * Create a timestamped value.
   *
   * @param timestamp timestamp in local time base
   * @param serverTime timestamp in server time base
   * @param value value
   */
  public TimestampedObject(long timestamp, long serverTime, T value) {
    this.timestamp = timestamp;
    this.serverTime = serverTime;
    this.value = value;
  }

  /** Timestamp in local time base. */
  @SuppressWarnings("MemberName")
  public final long timestamp;

  /** Timestamp in server time base. May be 0 or 1 for locally set values. */
  @SuppressWarnings("MemberName")
  public final long serverTime;

  /** Value. */
  @SuppressWarnings("MemberName")
  public final T value;
}
