/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.controller;

public class ArmFeedforward {
  private final double m_kS;
  private final double m_kCos;
  private final double m_kV;
  private final double m_kA;

  public ArmFeedforward(double kS, double kCos, double kV, double kA) {
    m_kS = kS;
    m_kCos = kCos;
    m_kV = kV;
    m_kA = kA;
  }

  public ArmFeedforward(double kS, double kCos, double kV) {
    this(kS, kCos, kV, 0);
  }

  public double calculate(double positionRadians, double velocity,
                          double acceleration) {
    return m_kS * Math.signum(velocity) + m_kCos * Math.cos(positionRadians) + m_kV * velocity
        + m_kA * acceleration;
  }

  public double calculate(double positionRadians, double velocity) {
    return calculate(positionRadians, velocity, 0);
  }
}
