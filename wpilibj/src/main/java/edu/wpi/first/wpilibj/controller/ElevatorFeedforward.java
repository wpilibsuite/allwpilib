/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

public class ElevatorFeedforward {
  private final double m_kS;
  private final double m_kG;
  private final double m_kV;
  private final double m_kA;

  public ElevatorFeedforward(double kS, double kG, double kV, double kA) {
    m_kS = kS;
    m_kG = kG;
    m_kV = kV;
    m_kA = kA;
  }

  public ElevatorFeedforward(double kS, double kG, double kV) {
    this(kS, kG, kV, 0);
  }

  public double calculate(double velocity, double acceleration) {
    return m_kS * Math.signum(velocity) + m_kG + m_kV * velocity + m_kA * acceleration;
  }

  public double calculate(double velocity) {
    return calculate(velocity, 0);
  }
}
