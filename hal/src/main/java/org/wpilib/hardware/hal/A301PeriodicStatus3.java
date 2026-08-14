// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Data decoded from an A301 periodic status 3 frame. */
public class A301PeriodicStatus3 {
  /** Absolute encoder position in rotations. */
  public final double absoluteEncoderPosition;

  /** HAL status associated with the read. */
  public final int status;

  /** CAN frame timestamp in microseconds. */
  public final long timestamp;

  /**
   * Constructs A301 periodic status 3 data (called from the HAL).
   *
   * @param absoluteEncoderPosition absolute encoder position in rotations
   * @param status HAL status associated with the read
   * @param timestamp CAN frame timestamp in microseconds
   */
  public A301PeriodicStatus3(double absoluteEncoderPosition, int status, long timestamp) {
    this.absoluteEncoderPosition = absoluteEncoderPosition;
    this.status = status;
    this.timestamp = timestamp;
  }
}
