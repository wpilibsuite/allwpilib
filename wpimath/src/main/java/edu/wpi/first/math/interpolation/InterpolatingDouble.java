/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.math.interpolation;

import edu.wpi.first.wpiutil.math.MathUtil;

public class InterpolatingDouble implements Interpolatable<Double> {

  public double m_value;

  public InterpolatingDouble(double value) {
    this.m_value = value;
  }

  @Override
  @SuppressWarnings("ParameterName")
  public Double interpolate(Double endValue, double t) {
    return m_value + (endValue - m_value) * MathUtil.clamp(t, 0, 1);
  }
}
