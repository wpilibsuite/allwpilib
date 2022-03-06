// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import java.util.TreeMap;

/**
 * Interpolating Tree Maps are used to get values at points that are not defined by making a guess
 * from points that are defined. This uses linear interpolation.
 */
public class InterpolatingTreeMap<K extends Number, V extends Number> extends TreeMap<K, V> {
  /**
   * @param key Lookup for a value (does not have to exist)
   * @return Double
   */
  public Double getInterpolated(K key) {
    V val = get(key);
    if (val == null) {
      K ceilingKey = ceilingKey(key);
      K floorKey = floorKey(key);

      if (ceilingKey == null && floorKey == null) {
        return null;
      }
      if (ceilingKey == null) {
        return get(floorKey).doubleValue();
      }
      if (floorKey == null) {
        return get(ceilingKey).doubleValue();
      }
      V floor = get(floorKey);
      V ceiling = get(ceilingKey);

      return interpolate(floor, ceiling, inverseInterpolate(ceilingKey, key, floorKey));
    } else {
      return val.doubleValue();
    }
  }

  public double interpolate(V val1, V val2, double d) {
    double dydx = val2.doubleValue() - val1.doubleValue();
    return dydx * d + val1.doubleValue();
  }

  public double inverseInterpolate(K up, K q, K down) {
    double upperToLower = up.doubleValue() - down.doubleValue();
    if (upperToLower <= 0) {
      return 0.0;
    }
    double queryToLower = q.doubleValue() - down.doubleValue();
    if (queryToLower <= 0) {
      return 0.0;
    }
    return queryToLower / upperToLower;
  }
}
