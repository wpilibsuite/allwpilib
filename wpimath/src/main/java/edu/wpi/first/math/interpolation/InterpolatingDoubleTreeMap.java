// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import java.util.TreeMap;

/**
 * Interpolating Tree Maps are used to get values at points that are not defined by making a guess
 * from points that are defined. This uses linear interpolation.
 */
public class InterpolatingDoubleTreeMap extends TreeMap<Double, Double> {
  /**
   * @param key Lookup for a value (does not have to exist)
   * @return Double
   */
  public Double getInterpolated(double key) {
    Double val = get(key);
    if (val == null) {
      Double ceilingKey = ceilingKey(key);
      Double floorKey = floorKey(key);

      if (ceilingKey == null && floorKey == null) {
        return null;
      }
      if (ceilingKey == null) {
        return get(floorKey);
      }
      if (floorKey == null) {
        return get(ceilingKey);
      }
      Double floor = get(floorKey);
      Double ceiling = get(ceilingKey);

      return interpolate(floor, ceiling, inverseInterpolate(ceilingKey, key, floorKey));
    } else return val;
  }

  public Double interpolate(Double val1, Double val2, double d) {
    double dydx = val2 - val1;
    return dydx * d + val1;
  }

  public double inverseInterpolate(Double up, double q, Double down) {
    double upperToLower = up - down;
    if (upperToLower <= 0) {
      return 0;
    }
    double queryToLower = q - down;
    if (queryToLower <= 0) {
      return 0;
    }
    return queryToLower / upperToLower;
  }
}
