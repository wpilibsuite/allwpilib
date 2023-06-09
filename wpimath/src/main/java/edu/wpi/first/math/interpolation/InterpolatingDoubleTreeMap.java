// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

import edu.wpi.first.math.MathUtil;
import java.util.TreeMap;

/**
 * Interpolating Tree Maps are used to get values at points that are not defined by making a guess
 * from points that are defined. This uses linear interpolation.
 */
public class InterpolatingDoubleTreeMap<K extends Number, V extends Number> {
  private final TreeMap<K, V> m_map = new TreeMap<>();

  /**
   * Inserts a key-value pair.
   *
   * @param key The key.
   * @param value The value.
   */
  public void put(K key, V value) {
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
  public Double get(K key) {
    V val = m_map.get(key);
    if (val == null) {
      K ceilingKey = m_map.ceilingKey(key);
      K floorKey = m_map.floorKey(key);

      if (ceilingKey == null && floorKey == null) {
        return null;
      }
      if (ceilingKey == null) {
        return m_map.get(floorKey).doubleValue();
      }
      if (floorKey == null) {
        return m_map.get(ceilingKey).doubleValue();
      }
      V floor = m_map.get(floorKey);
      V ceiling = m_map.get(ceilingKey);

      return MathUtil.interpolate(
          floor.doubleValue(),
          ceiling.doubleValue(),
          MathUtil.inverseInterpolate(
              floorKey.doubleValue(), ceilingKey.doubleValue(), key.doubleValue()));
    } else {
      return val.doubleValue();
    }
  }

  /** Clears the contents. */
  public void clear() {
    m_map.clear();
  }

  /**
   * Return where within interpolation range [0, 1] q is between down and up.
   *
   * @param up Upper part of interpolation range.
   * @param q Query.
   * @param down Lower part of interpolation range.
   * @return Interpolant in range [0, 1].
   */
  private double inverseInterpolate(K up, K q, K down) {
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
