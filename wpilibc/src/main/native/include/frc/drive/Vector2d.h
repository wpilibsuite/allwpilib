// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

/**
 * This is a 2D vector struct that supports basic vector operations.
 */
struct Vector2d {
  Vector2d() = default;
  Vector2d(double x, double y);

  /**
   * Rotate a vector in Cartesian space.
   *
   * @param angle angle in degrees by which to rotate vector counter-clockwise.
   */
  void Rotate(double angle);

  /**
   * Returns dot product of this vector with argument.
   *
   * @param vec Vector with which to perform dot product.
   */
  double Dot(const Vector2d& vec) const;

  /**
   * Returns magnitude of vector.
   */
  double Magnitude() const;

  /**
   * Returns scalar projection of this vector onto argument.
   *
   * @param vec Vector onto which to project this vector.
   */
  double ScalarProject(const Vector2d& vec) const;

  double x = 0.0;
  double y = 0.0;
};

}  // namespace frc
