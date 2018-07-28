/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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
