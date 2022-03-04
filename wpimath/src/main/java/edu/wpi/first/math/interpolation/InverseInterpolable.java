// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.interpolation;

/**
 * InverseInterpolable is an interface used by an Interpolating Tree as the Key type. Given two
 * endpoint keys and a third query key, an InverseInterpolable object can calculate the
 * interpolation parameter of the query key on the interval [0, 1].
 *
 * @param <T> The Type of InverseInterpolable
 * @see InterpolatingTreeMap
 */
public interface InverseInterpolable<T> {
  /**
   * Given this point (lower), a query point (query), and an upper point (upper), estimate how far
   * (on [0, 1]) between 'lower' and 'upper' the query point lies.
   *
   * @param upper upper point
   * @param query a query point
   * @return The interpolation parameter on [0, 1] representing how far between this point and the
   *     upper point the query point lies.
   */
  public double inverseInterpolate(T upper, T query);
}
