/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

/**
 * A helper class that computes feedforward outputs for a simple permanent-magnet DC motor.
 */
public class SimpleMotorFeedforward {
  private final double m_ks;
  private final double m_kv;
  private final double m_ka;

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains.  Units of the gain values
   * will dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kv The velocity gain.
   * @param ka The acceleration gain.
   */
  public SimpleMotorFeedforward(double ks, double kv, double ka) {
    m_ks = ks;
    m_kv = kv;
    m_ka = ka;
  }

  /**
   * Creates a new SimpleMotorFeedforward with the specified gains.  Acceleration gain is
   * defaulted to zero.  Units of the gain values will dictate units of the computed feedforward.
   *
   * @param ks The static gain.
   * @param kv The velocity gain.
   */
  public SimpleMotorFeedforward(double ks, double kv) {
    this(ks, kv, 0);
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity     The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity, double acceleration) {
    return m_ks * Math.signum(velocity) + m_kv * velocity + m_ka * acceleration;
  }

  /**
   * Calculates the feedforward from the gains and velocity setpoint (acceleration is assumed to
   * be zero).
   *
   * @param velocity The velocity setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity) {
    return calculate(velocity, 0);
  }
}
