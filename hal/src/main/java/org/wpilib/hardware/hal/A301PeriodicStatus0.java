// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Data decoded from an A301 periodic status 0 frame. */
public class A301PeriodicStatus0 {
  public final double appliedOutput;
  public final double voltage;
  public final double current;
  public final int motorTemperature;
  public final boolean inverted;
  public final boolean primaryHeartbeatLock;
  public final int gearboxRPM;
  public final int status;
  public final long timestamp;

  /** Constructs A301 periodic status 0 data (called from the HAL). */
  public A301PeriodicStatus0(
      double appliedOutput,
      double voltage,
      double current,
      int motorTemperature,
      boolean inverted,
      boolean primaryHeartbeatLock,
      int gearboxRPM,
      int status,
      long timestamp) {
    this.appliedOutput = appliedOutput;
    this.voltage = voltage;
    this.current = current;
    this.motorTemperature = motorTemperature;
    this.inverted = inverted;
    this.primaryHeartbeatLock = primaryHeartbeatLock;
    this.gearboxRPM = gearboxRPM;
    this.status = status;
    this.timestamp = timestamp;
  }
}
