/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

namespace frc {

constexpr double kPi = 3.14159265358979323846;

/**
 * A rotation in a 2d coordinate frame represented a point on the unit circle
 * (cosine and sine).
 */
class Rotation2d {
 public:
  /**
   * Constructs a Rotation2d with a default angle of 0 degrees.
   */
  Rotation2d();

  /**
   * Constructs a Rotation2d with the given radian value.
   */
  explicit Rotation2d(double value);

  /**
   * Constructs a Rotation2d with the given x and y (cosine and sine)
   * components.
   */
  Rotation2d(double x, double y);

  /**
   * Constructs and returns a Rotation2d with the given degree value.
   */
  static Rotation2d FromDegrees(double degrees);

  /**
   * Adds two rotations together, with the result being bounded between -kPi and
   * kPi.
   *
   * For example, Rotation2d.FromDegrees(30) + Rotation2d.FromDegrees(60) =
   * Rotation2d{-kPi/2}
   */
  Rotation2d operator+(const Rotation2d& other) const;

  /**
   * Adds a rotation to the current rotation.
   *
   * This is similar to the + operator except that it mutates the current
   * object.
   */
  Rotation2d& operator+=(const Rotation2d& other);

  /**
   * Subtracts the new rotation from the current rotation and returns the new
   * rotation.
   *
   * For example, Rotation2d.FromDegrees(10) - Rotation2d.FromDegrees(100) =
   * Rotation2d{-kPi/2}
   */
  Rotation2d operator-(const Rotation2d& other) const;

  /**
   * Subtracts the new rotation from the current rotation.
   *
   * This is similar to the - operator except that it mutates the current
   * object.
   */
  Rotation2d& operator-=(const Rotation2d& other);

  /**
   * Takes the inverse of the current rotation. This is simply the negative of
   * the current angular value.
   */
  Rotation2d operator-() const;

  /**
   * Adds the new rotation to the current rotation using a rotation matrix.
   *
   * [cos_new] = [other.cos, -other.sin][cos]
   * [sin_new] = [other.sin,  other.cos][sin]
   *
   * value_new = std::atan2(cos_new, sin_new)
   *
   */
  Rotation2d RotateBy(const Rotation2d& other) const;

  /*
   * Returns the radian value of the rotation.
   */
  double Radians() const { return m_value; }

  /**
   * Returns the degree value of the rotation.
   */
  double Degrees() const { return Rad2Deg(m_value); }

  /**
   * Returns the cosine of the rotation.
   */
  double Cos() const { return m_cos; }

  /**
   * Returns the sine of the rotation.
   */
  double Sin() const { return m_sin; }

  /**
   * Returns the tangent of the rotation.
   */
  double Tan() const { return m_sin / m_cos; }

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
}  // namespace frc
