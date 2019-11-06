/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

/**
 * A helper class that computes feedforward outputs for a simple elevator (modeled as a motor
 * acting against the force of gravity).
 */
public class ElevatorFeedforward {
  private final double m_kS;
  private final double m_kG;
  private final double m_kV;
  private final double m_kA;

  /**
   * Creates a new ElevatorFeedforward with the specified gains.  Units of the gain values
   * will dictate units of the computed feedforward.
   *
   * @param kS The static gain.
   * @param kG The gravity gain.
   * @param kV The velocity gain.
   * @param kA The acceleration gain.
   */
  public ElevatorFeedforward(double kS, double kG, double kV, double kA) {
    m_kS = kS;
    m_kG = kG;
    m_kV = kV;
    m_kA = kA;
  }

  /**
   * Creates a new ElevatorFeedforward with the specified gains.  Acceleration gain is
   * defaulted to zero.  Units of the gain values will dictate units of the computed feedforward.
   *
   * @param kS The static gain.
   * @param kG The gravity gain.
   * @param kV The velocity gain.
   */
  public ElevatorFeedforward(double kS, double kG, double kV) {
    this(kS, kG, kV, 0);
  }

  /**
   * Calculates the feedforward from the gains and setpoints.
   *
   * @param velocity     The velocity setpoint.
   * @param acceleration The acceleration setpoint.
   * @return The computed feedforward.
   */
  public double calculate(double velocity, double acceleration) {
    return m_kS * Math.signum(velocity) + m_kG + m_kV * velocity + m_kA * acceleration;
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
