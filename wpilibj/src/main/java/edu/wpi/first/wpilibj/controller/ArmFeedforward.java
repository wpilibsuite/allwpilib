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
public class ArmFeedforward {
  private final double m_ks;
  private final double m_kcos;
  private final double m_kv;
  private final double m_ka;

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
    m_ks = ks;
    m_kcos = kcos;
    m_kv = kv;
    m_ka = ka;
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
    return m_ks * Math.signum(velocityRadPerSec) + m_kcos * Math.cos(positionRadians)
        + m_kv * velocityRadPerSec
        + m_ka * accelRadPerSecSquared;
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
