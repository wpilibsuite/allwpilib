/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

/**
 * A helper class that computes feedforward outputs for a simple arm (modeled as a motor
 * acting against the force of gravity on a beam suspended at an angle).
 */
@SuppressWarnings("MemberName")
public class ArmFeedforward {
  public final double ks;
  public final double kcos;
  public final double kv;
  public final double ka;

  /**
   * Creates a new ArmFeedforward with the specified gains.  Units of the gain values
   * will dictate units of the computed feedforward.
   *
   * @param ks   The static gain.
   * @param kcos The gravity gain.
   * @param kv   The velocity gain.
   * @param ka   The acceleration gain.
   */
  public ArmFeedforward(double ks, double kcos, double kv, double ka) {
    this.ks = ks;
    this.kcos = kcos;
    this.kv = kv;
    this.ka = ka;
  }

  /**
   * Creates a new ArmFeedforward with the specified gains.  Acceleration gain is
   * defaulted to zero.  Units of the gain values will dictate units of the computed feedforward.
   *
   * @param ks   The static gain.
   * @param kcos The gravity gain.
   * @param kv   The velocity gain.
   */
  public ArmFeedforward(double ks, double kcos, double kv) {
    this(ks, kcos, kv, 0);
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocityRadPerSec     The velocity setpoint.
   * @param accelRadPerSecSquared The acceleration setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double positionRadians, double velocityRadPerSec,
                          double accelRadPerSecSquared) {
    return ks * Math.signum(velocityRadPerSec) + kcos * Math.cos(positionRadians)
        + kv * velocityRadPerSec
        + ka * accelRadPerSecSquared;
  }

  /**
   * Calculates the feedforward from the gains and velocity setpoint (acceleration is assumed to
   * be zero).
   *
   * @param velocity The velocity setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double positionRadians, double velocity) {
    return calculate(positionRadians, velocity, 0);
  }
}
