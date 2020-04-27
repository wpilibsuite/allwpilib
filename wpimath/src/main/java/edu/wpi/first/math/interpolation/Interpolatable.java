/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.math.interpolation;

/**
 * An object should extend interpolatable if you wish to linearly interpolate between a lower and
 * upper bound, such as a robot position on the field between timesteps.
 * @param <T> The class that is interpolatable.
 */
public interface Interpolatable<T> {

  /**
   * Return the interpolated value. This object is assumed to be the starting position,
   * or lower bound.
   * @param endValue The upper bound, or end.
   * @param t How far between the lower and upper bound we are. This should be bounded in [0, 1].
   * @return
   */
  @SuppressWarnings("ParameterName")
  T interpolate(T endValue, double t);
}
