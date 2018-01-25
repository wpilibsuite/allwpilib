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

  void Rotate(double angle);
  double Dot(const Vector2d& vec) const;
  double Magnitude() const;
  double ScalarProject(const Vector2d& vec) const;

  double x = 0.0;
  double y = 0.0;
};

}  // namespace frc
