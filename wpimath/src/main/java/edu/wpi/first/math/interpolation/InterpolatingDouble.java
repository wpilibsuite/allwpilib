// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

/**
 * A Double that can be interpolated using the InterpolatingTreeMap.
 *
 * @see InterpolatingTreeMap
 */
public class InterpolatingDouble
    implements Interpolatable<InterpolatingDouble>,
        InverseInterpolable<InterpolatingDouble>,
        Comparable<InterpolatingDouble> {
  public double value;

  public InterpolatingDouble(double val) {
    value = val;
  }

  @Override
  public InterpolatingDouble interpolate(InterpolatingDouble other, double x) {
    double dydx = other.value - value;
    double searchY = dydx * x + value;
    return new InterpolatingDouble(searchY);
  }

  @Override
  public double inverseInterpolate(InterpolatingDouble upper, InterpolatingDouble query) {
    double upperToLower = upper.value - value;
    if (upperToLower <= 0) {
      return 0;
    }
    double queryToLower = query.value - value;
    if (queryToLower <= 0) {
      return 0;
    }
    return queryToLower / upperToLower;
  }

  @Override
  public int compareTo(InterpolatingDouble other) {
    return Double.compare(value, other.value);
  }

  @Override
  public boolean equals(Object o) {
    if (this == o) {
      return true;
    }
    if (o == null || getClass() != o.getClass()) {
      return false;
    }
    InterpolatingDouble that = (InterpolatingDouble) o;
    return that.value == this.value;
  }
}
