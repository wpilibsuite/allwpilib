// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math;

import java.util.TreeMap;

/**
 * Interpolating Tree Maps are used to get values at points that are not defined by making a guess
 * from points that are defined. This uses linear interpolation.
 */
public class InterpolatingMatrixTreeMap<K extends Number, R extends Num, C extends Num> {
  private final TreeMap<K, Matrix<R, C>> m_map = new TreeMap<>();

  /**
   * Inserts a key-value pair.
   *
   * @param key The key.
   * @param value The value.
   */
  public void put(K key, Matrix<R, C> value) {
    m_map.put(key, value);
  }

  /**
   * Returns the value associated with a given key.
   *
   * <p>If there's no matching key, the value returned will be a linear interpolation between the
   * keys before and after the provided one.
   *
   * @param key The key.
   * @return The value associated with the given key.
   */
  public Matrix<R, C> get(K key) {
    Matrix<R, C> val = m_map.get(key);
    if (val == null) {
      K ceilingKey = m_map.ceilingKey(key);
      K floorKey = m_map.floorKey(key);

      if (ceilingKey == null && floorKey == null) {
        return null;
      }
      if (ceilingKey == null) {
        return m_map.get(floorKey);
      }
      if (floorKey == null) {
        return m_map.get(ceilingKey);
      }
      Matrix<R, C> floor = m_map.get(floorKey);
      Matrix<R, C> ceiling = m_map.get(ceilingKey);

      return interpolate(floor, ceiling, inverseInterpolate(ceilingKey, key, floorKey));
    } else {
      return val;
    }
  }

  /**
   * Return the value interpolated between val1 and val2 by the interpolant d.
   *
   * @param val1 The lower part of the interpolation range.
   * @param val2 The upper part of the interpolation range.
   * @param d The interpolant in the range [0, 1].
   * @return The interpolated value.
   */
  public Matrix<R, C> interpolate(Matrix<R, C> val1, Matrix<R, C> val2, double d) {
    var dydx = val2.minus(val1);
    return dydx.times(d).plus(val1);
  }

  /**
   * Return where within interpolation range [0, 1] q is between down and up.
   *
   * @param up Upper part of interpolation range.
   * @param q Query.
   * @param down Lower part of interpolation range.
   * @return Interpolant in range [0, 1].
   */
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
