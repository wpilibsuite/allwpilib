/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Rotation2d.h"

namespace frc {
namespace experimental {

class Translation2d {
 public:
  Translation2d();
  Translation2d(double x, double y);

  double Distance(const Translation2d& other) const;

  double X() const { return m_x; }
  double Y() const { return m_y; }
  double Norm() const;

  Translation2d RotateBy(const Rotation2d& other) const;

  Translation2d operator+(const Translation2d& other) const;
  void operator+=(const Translation2d& other);

  Translation2d operator-(const Translation2d& other) const;
  void operator-=(const Translation2d& other);

  Translation2d operator-() const;

  Translation2d operator*(double scalar) const;
  void operator*=(double scalar);

  Translation2d operator/(double scalar) const;
  void operator/=(double scalar);

 private:
  double m_x;
  double m_y;
};
}  // namespace experimental
}  // namespace frc
