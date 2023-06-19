// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

/**
 * Interpolating Tree Maps are used to get values at points that are not defined by making a guess
 * from points that are defined. This uses linear interpolation.
 */
public class InterpolatingDoubleTreeMap<K extends Number, V extends Number> {
  private final InterpolatingTreeMap<Double, Double> m_map =
      new InterpolatingTreeMap<>(InverseInterpolator.forDouble(), Interpolator.forDouble());

  /**
   * Inserts a key-value pair.
   *
   * @param key The key.
   * @param value The value.
   */
  public void put(K key, V value) {
    m_map.put(key.doubleValue(), value.doubleValue());
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
    return m_map.get(key.doubleValue());
  }

  /** Clears the contents. */
  public void clear() {
    m_map.clear();
  }
}
