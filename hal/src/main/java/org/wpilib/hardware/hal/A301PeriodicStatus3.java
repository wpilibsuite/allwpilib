// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Data decoded from an A301 periodic status 3 frame. */
public class A301PeriodicStatus3 {
  public final double absoluteEncoderPosition;
  public final int status;
  public final long timestamp;

  /** Constructs A301 periodic status 3 data (called from the HAL). */
  public A301PeriodicStatus3(double absoluteEncoderPosition, int status, long timestamp) {
    this.absoluteEncoderPosition = absoluteEncoderPosition;
    this.status = status;
    this.timestamp = timestamp;
  }
}
