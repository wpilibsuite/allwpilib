// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.kinematics;

import edu.wpi.first.math.interpolation.Interpolatable;

public interface WheelPositions<T extends WheelPositions<T>> extends Interpolatable<T> {
  /**
   * Returns a copy of this instance.
   *
   * @return A copy.
   */
  T copy();

  /**
   * Returns a representation of how the wheels moved from other to this.
   *
   * @param other The other instance to compare to.
   * @return The representation of how the wheels moved from other to this.
   */
  T minus(T other);
}
