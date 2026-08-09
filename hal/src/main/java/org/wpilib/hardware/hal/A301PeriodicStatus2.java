// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Data decoded from an A301 periodic status 2 frame. */
public class A301PeriodicStatus2 {
  public final double encoderVelocity;
  public final double relativeEncoderPosition;
  public final int status;
  public final long timestamp;

  /**
   * Constructs A301 periodic status 2 data (called from the HAL).
   *
   * @param encoderVelocity relative encoder velocity in RPM
   * @param relativeEncoderPosition relative encoder position in motor rotations
   * @param status HAL status associated with the read
   * @param timestamp CAN frame timestamp in microseconds
   */
  public A301PeriodicStatus2(
      double encoderVelocity, double relativeEncoderPosition, int status, long timestamp) {
    this.encoderVelocity = encoderVelocity;
    this.relativeEncoderPosition = relativeEncoderPosition;
    this.status = status;
    this.timestamp = timestamp;
  }
}
