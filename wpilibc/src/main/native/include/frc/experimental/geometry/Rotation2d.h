/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {
namespace experimental {

constexpr double kPi = 3.14159265358979323846;

class Rotation2d {
 public:
  Rotation2d();
  explicit Rotation2d(double value);
  Rotation2d(double x, double y);

  static Rotation2d FromDegrees(double degrees);

  Rotation2d operator+(const Rotation2d& other) const;
  void operator+=(const Rotation2d& other);

  Rotation2d operator-(const Rotation2d& other) const;
  void operator-=(const Rotation2d& other);

  Rotation2d operator-() const;

  Rotation2d RotateBy(const Rotation2d& other) const;

  double Radians() const { return m_value; };
  double Degrees() const { return Rad2Deg(m_value); }
  double Cos() const { return m_cos; };
  double Sin() const { return m_sin; };
  double Tan() const { return m_sin / m_cos; };

 private:
  double m_value;
  double m_cos;
  double m_sin;

  template <typename T>
  static constexpr T Rad2Deg(const T& rad) {
    return rad * 180.0 / kPi;
  }

  template <typename T>
  static constexpr T Deg2Rad(const T& deg) {
    return deg * kPi / 180.0;
  }
};
}  // namespace experimental
}  // namespace frc
