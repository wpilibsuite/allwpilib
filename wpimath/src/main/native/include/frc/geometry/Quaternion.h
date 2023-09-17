// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <wpi/SymbolExports.h>

namespace wpi {
class json;
}  // namespace wpi

namespace frc {

class WPILIB_DLLEXPORT Quaternion {
 public:
  /**
   * Constructs a quaternion with a default angle of 0 degrees.
   */
  Quaternion() = default;

  /**
   * Constructs a quaternion with the given components.
   *
   * @param w W component of the quaternion.
   * @param x X component of the quaternion.
   * @param y Y component of the quaternion.
   * @param z Z component of the quaternion.
   */
  Quaternion(double w, double x, double y, double z);

  /**
   * Adds with another quaternion.
   *
   * @param other the other quaternion
   */
  Quaternion operator+(const Quaternion& other) const;

  /**
   * Subtracts another quaternion.
   *
   * @param other the other quaternion
   */
  Quaternion operator-(const Quaternion& other) const;

  /**
   * Multiples with a scalar value.
   *
   * @param other the scalar value
   */
  Quaternion operator*(const double other) const;

  /**
   * Divides by a scalar value.
   *
   * @param other the scalar value
   */
  Quaternion operator/(const double other) const;

  /**
   * Multiply with another quaternion.
   *
   * @param other The other quaternion.
   */
  Quaternion operator*(const Quaternion& other) const;

  /**
   * Checks equality between this Quaternion and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const Quaternion& other) const;

  /**
   * Returns the conjugate of the quaternion.
   */
  Quaternion Conjugate() const;

  /**
   * Returns the inverse of the quaternion.
   */
  Quaternion Inverse() const;

  /**
   * Normalizes the quaternion.
   */
  Quaternion Normalize() const;

  /**
   * Calculates the L2 norm of the quaternion.
   */
  double Norm() const;

  /**
   * Calculates this quaternion raised to a power.
   *
   * @param t the power to raise this quaternion to.
   */
  Quaternion Pow(const double t) const;

  /**
   * Matrix exponential of a quaternion.
   *
   * @param other the "Twist" that will be applied to this quaternion.
   */
  Quaternion Exp(const Quaternion& other) const;

  /**
   * Matrix exponential of a quaternion.
   *
   * source:
   * https://en.wikipedia.org/wiki/Quaternion#Exponential,_logarithm,_and_power_functions
   */
  Quaternion Exp() const;

  /**
   * Log operator of a quaternion.
   *
   * @param other The quaternion to map this quaternion onto
   */
  Quaternion Log(const Quaternion& other) const;

  /**
   * Log operator of a quaternion.
   *
   * source:
   * https://en.wikipedia.org/wiki/Quaternion#Exponential,_logarithm,_and_power_functions
   */
  Quaternion Log() const;

  /**
   * Returns W component of the quaternion.
   */
  double W() const;

  /**
   * Returns X component of the quaternion.
   */
  double X() const;

  /**
   * Returns Y component of the quaternion.
   */
  double Y() const;

  /**
   * Returns Z component of the quaternion.
   */
  double Z() const;

  /**
   * Returns the rotation vector representation of this quaternion.
   *
   * This is also the log operator of SO(3).
   */
  Eigen::Vector3d ToRotationVector() const;

 private:
  // Scalar r in versor form
  double m_r = 1.0;

  // Vector v in versor form
  Eigen::Vector3d m_v{0.0, 0.0, 0.0};
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Quaternion& quaternion);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Quaternion& quaternion);

}  // namespace frc
