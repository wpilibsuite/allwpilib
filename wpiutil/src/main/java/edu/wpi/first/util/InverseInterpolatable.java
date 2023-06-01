package edu.wpi.first.util;

/**
 * InverseInterpolatable allows an object to determine where within an interpolation range it lies.
 *
 * @param <T> The class that is inverse interpolatable.
 */
public interface InverseInterpolatable<T> {

  /**
   * Return how far between lower and upper bound this object is.
   *
   * @param lower The lower bound of the interpolation range.
   * @param upper The upper bound of the interpolation range.
   * @return Interpolant in range [0, 1].
   */
  double inverseInterpolate(T lower, T upper);
}
