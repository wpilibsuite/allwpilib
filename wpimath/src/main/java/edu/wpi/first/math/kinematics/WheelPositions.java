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
   * Returns the difference with another set of wheel positions.
   *
   * @param other The other instance to compare to.
   * @return The representation of how the wheels moved from {@link other} to this.
   */
  T minus(T other);
}
