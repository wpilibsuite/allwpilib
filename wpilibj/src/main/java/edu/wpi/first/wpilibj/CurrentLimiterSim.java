// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.math.system.plant.DCMotor;

/** A utility class for simulating predictive current limiting. */
public class CurrentLimiterSim {
  // The type and number of motors.
  private final DCMotor m_gearbox;

  // The reduction on the mechanism being simulated.
  private final double m_reduction;

  // The current limit of the motor; 0 represents no current limit.
  private double m_currentLimit;

  /**
   * Creates a new CurrentLimiterSim.
   *
   * @param gearbox The type and number of motors
   * @param reduction The gearing on the mechanism being simulated
   * @param currentLimit The current limit to apply
   */
  public CurrentLimiterSim(DCMotor gearbox, double reduction, double currentLimit) {
    m_gearbox = gearbox;
    m_reduction = reduction;
    setCurrentLimit(currentLimit);
  }

  /**
   * Changes the current limit to a new value.
   *
   * @param currentLimit The current limit to apply
   */
  public void setCurrentLimit(double currentLimit) {
    m_currentLimit = currentLimit;
  }

  /**
   * Returns a new voltage that respects the current limit given.
   *
   * @param appliedVoltage The initial voltage being requested
   * @param angularVelocity The current angular velocity of the mechanism
   * @return A limited voltage that respects the current limit
   */
  public double limitVoltage(double appliedVoltage, double angularVelocity) {
    double appliedCurrent = m_gearbox.getCurrent(angularVelocity * m_reduction, appliedVoltage);
    double limitedVolts = appliedVoltage;
    if (Math.abs(appliedCurrent) > m_currentLimit) {
      limitedVolts =
          m_gearbox.getVoltage(
              m_gearbox.getTorque(Math.copySign(m_currentLimit, appliedCurrent)),
              angularVelocity * m_reduction);
    }
    // ensure the current limit doesn't cause an increase to output voltage
    if (Math.abs(limitedVolts) < Math.abs(appliedVoltage)) {
      return limitedVolts;
    } else {
      return appliedVoltage;
    }
  }
}
