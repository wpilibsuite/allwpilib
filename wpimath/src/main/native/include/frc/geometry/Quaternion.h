// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/EigenCore.h"

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
   * Returns the inverse of the quaternion.
   */
  Quaternion Inverse() const;

  /**
   * Normalizes the quaternion.
   */
  Quaternion Normalize() const;

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
  double m_r = 1.0;
  Eigen::Vector3d m_v{0.0, 0.0, 0.0};
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Quaternion& quaternion);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Quaternion& quaternion);

}  // namespace frc
