// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Structure for holding the values stored in an accumulator. */
@SuppressWarnings("MemberName")
public class AccumulatorResult {
  /** The total value accumulated. */
  public long value;
  /** The number of sample value was accumulated over. */
  public long count;

  /**
   * Set the value and count.
   *
   * @param value The total value accumulated.
   * @param count The number of samples accumulated.
   */
  public void set(long value, long count) {
    this.value = value;
    this.count = count;
  }
}
