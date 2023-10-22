// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.interpolation.Interpolatable;

/**
 * Interface for wheel positions.
 *
 * @param <T> Wheel positions type.
 */
public interface WheelPositions<T extends WheelPositions<T>> extends Interpolatable<T> {
  /**
   * Returns a copy of this instance.
   *
   * @return A copy.
   */
  T copy();
}
