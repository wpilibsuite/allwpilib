// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/** Data decoded from an A301 periodic status 0 frame. */
public class A301PeriodicStatus0 {
  /** Applied output duty cycle. */
  public final double appliedOutput;

  /** Controller input voltage in volts. */
  public final double voltage;

  /** Motor current in amperes. */
  public final double current;

  /** Motor temperature in degrees Celsius. */
  public final int motorTemperature;

  /** Whether non-position control output is inverted. */
  public final boolean inverted;

  /** Whether the primary heartbeat is locked. */
  public final boolean primaryHeartbeatLock;

  /** Gearbox speed variant. */
  public final int gearboxRPM;

  /** HAL status associated with the read. */
  public final int status;

  /** CAN frame timestamp in microseconds. */
  public final long timestamp;

  /**
   * Constructs A301 periodic status 0 data (called from the HAL).
   *
   * @param appliedOutput applied output duty cycle
   * @param voltage controller input voltage in volts
   * @param current motor current in amperes
   * @param motorTemperature motor temperature in degrees Celsius
   * @param inverted whether non-position control output is inverted
   * @param primaryHeartbeatLock whether the primary heartbeat is locked
   * @param gearboxRPM gearbox speed variant
   * @param status HAL status associated with the read
   * @param timestamp CAN frame timestamp in microseconds
   */
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
