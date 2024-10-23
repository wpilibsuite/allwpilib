// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <numbers>

#include <Eigen/Core>
#include <gcem.hpp>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

namespace frc {

/**
 * Represents a quaternion.
 */
class WPILIB_DLLEXPORT Quaternion {
 public:
  /**
   * Constructs a quaternion with a default angle of 0 degrees.
   */
  constexpr Quaternion() = default;

  /**
   * Constructs a quaternion with the given components.
   *
   * @param w W component of the quaternion.
   * @param x X component of the quaternion.
   * @param y Y component of the quaternion.
   * @param z Z component of the quaternion.
   */
  constexpr Quaternion(double w, double x, double y, double z)
      : m_w{w}, m_x{x}, m_y{y}, m_z{z} {}

  /**
   * Adds with another quaternion.
   *
   * @param other the other quaternion
   */
  constexpr Quaternion operator+(const Quaternion& other) const {
    return Quaternion{m_w + other.m_w, m_x + other.m_x, m_y + other.m_y,
                      m_z + other.m_z};
  }

  /**
   * Subtracts another quaternion.
   *
   * @param other the other quaternion
   */
  constexpr Quaternion operator-(const Quaternion& other) const {
    return Quaternion{m_w - other.m_w, m_x - other.m_x, m_y - other.m_y,
                      m_z - other.m_z};
  }

  /**
   * Multiples with a scalar value.
   *
   * @param other the scalar value
   */
  constexpr Quaternion operator*(double other) const {
    return Quaternion{m_w * other, m_x * other, m_y * other, m_z * other};
  }

  /**
   * Divides by a scalar value.
   *
   * @param other the scalar value
   */
  constexpr Quaternion operator/(double other) const {
    return Quaternion{m_w / other, m_x / other, m_y / other, m_z / other};
  }

  /**
   * Multiply with another quaternion.
   *
   * @param other The other quaternion.
   */
  constexpr Quaternion operator*(const Quaternion& other) const {
    // https://en.wikipedia.org/wiki/Quaternion#Scalar_and_vector_parts
    const auto& r1 = m_w;
    const auto& r2 = other.m_w;

    // v₁ ⋅ v₂
    double dot = m_x * other.m_x + m_y * other.m_y + m_z * other.m_z;

    // v₁ x v₂
    double cross_x = m_y * other.m_z - other.m_y * m_z;
    double cross_y = other.m_x * m_z - m_x * other.m_z;
    double cross_z = m_x * other.m_y - other.m_x * m_y;

    return Quaternion{// r = r₁r₂ − v₁ ⋅ v₂
                      r1 * r2 - dot,
                      // v = r₁v₂ + r₂v₁ + v₁ x v₂
                      r1 * other.m_x + r2 * m_x + cross_x,
                      r1 * other.m_y + r2 * m_y + cross_y,
                      r1 * other.m_z + r2 * m_z + cross_z};
  }

  /**
   * Checks equality between this Quaternion and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  constexpr bool operator==(const Quaternion& other) const {
    return gcem::abs(Dot(other) - Norm() * other.Norm()) < 1e-9 &&
           gcem::abs(Norm() - other.Norm()) < 1e-9;
  }

  /**
   * Returns the elementwise product of two quaternions.
   */
  constexpr double Dot(const Quaternion& other) const {
    return W() * other.W() + X() * other.X() + Y() * other.Y() +
           Z() * other.Z();
  }

  /**
   * Returns the conjugate of the quaternion.
   */
  constexpr Quaternion Conjugate() const {
    return Quaternion{W(), -X(), -Y(), -Z()};
  }

  /**
   * Returns the inverse of the quaternion.
   */
  constexpr Quaternion Inverse() const {
    double norm = Norm();
    return Conjugate() / (norm * norm);
  }

  /**
   * Normalizes the quaternion.
   */
  constexpr Quaternion Normalize() const {
    double norm = Norm();
    if (norm == 0.0) {
      return Quaternion{};
    } else {
      return Quaternion{W(), X(), Y(), Z()} / norm;
    }
  }

  /**
   * Calculates the L2 norm of the quaternion.
   */
  constexpr double Norm() const { return gcem::sqrt(Dot(*this)); }

  /**
   * Calculates this quaternion raised to a power.
   *
   * @param t the power to raise this quaternion to.
   */
  constexpr Quaternion Pow(double t) const { return (Log() * t).Exp(); }

  /**
   * Matrix exponential of a quaternion.
   *
   * @param other the "Twist" that will be applied to this quaternion.
   */
  constexpr Quaternion Exp(const Quaternion& other) const {
    return other.Exp() * *this;
  }

  /**
   * Matrix exponential of a quaternion.
   *
   * source: wpimath/algorithms.md
   *
   *  If this quaternion is in 𝖘𝖔(3) and you are looking for an element of
   * SO(3), use FromRotationVector
   */
  constexpr Quaternion Exp() const {
    double scalar = gcem::exp(m_w);

    double axial_magnitude = gcem::hypot(m_x, m_y, m_z);
    double cosine = gcem::cos(axial_magnitude);

    double axial_scalar;

    if (axial_magnitude < 1e-9) {
      // Taylor series of sin(x)/x near x=0: 1 − x²/6 + x⁴/120 + O(n⁶)
      double axial_magnitude_sq = axial_magnitude * axial_magnitude;
      double axial_magnitude_sq_sq = axial_magnitude_sq * axial_magnitude_sq;
      axial_scalar =
          1.0 - axial_magnitude_sq / 6.0 + axial_magnitude_sq_sq / 120.0;
    } else {
      axial_scalar = gcem::sin(axial_magnitude) / axial_magnitude;
    }

    return Quaternion(cosine * scalar, X() * axial_scalar * scalar,
                      Y() * axial_scalar * scalar, Z() * axial_scalar * scalar);
  }

  /**
   * Log operator of a quaternion.
   *
   * @param other The quaternion to map this quaternion onto
   */
  constexpr Quaternion Log(const Quaternion& other) const {
    return (other * Inverse()).Log();
  }

  /**
   * Log operator of a quaternion.
   *
   * source:  wpimath/algorithms.md
   *
   * If this quaternion is in SO(3) and you are looking for an element of 𝖘𝖔(3),
   * use ToRotationVector
   */
  constexpr Quaternion Log() const {
    double norm = Norm();
    double scalar = gcem::log(norm);

    double v_norm = gcem::hypot(m_x, m_y, m_z);

    double s_norm = W() / norm;

    if (gcem::abs(s_norm + 1) < 1e-9) {
      return Quaternion{scalar, -std::numbers::pi, 0, 0};
    }

    double v_scalar;

    if (v_norm < 1e-9) {
      // Taylor series expansion of atan2(y/x)/y at y = 0:
      //
      //   1/x - 1/3 y²/x³ + O(y⁴)
      v_scalar = 1.0 / W() - 1.0 / 3.0 * v_norm * v_norm / (W() * W() * W());
    } else {
      v_scalar = gcem::atan2(v_norm, W()) / v_norm;
    }

    return Quaternion{scalar, v_scalar * m_x, v_scalar * m_y, v_scalar * m_z};
  }

  /**
   * Returns W component of the quaternion.
   */
  constexpr double W() const { return m_w; }

  /**
   * Returns X component of the quaternion.
   */
  constexpr double X() const { return m_x; }

  /**
   * Returns Y component of the quaternion.
   */
  constexpr double Y() const { return m_y; }

  /**
   * Returns Z component of the quaternion.
   */
  constexpr double Z() const { return m_z; }

  /**
   * Returns the rotation vector representation of this quaternion.
   *
   * This is also the log operator of SO(3).
   */
  constexpr Eigen::Vector3d ToRotationVector() const {
    // See equation (31) in "Integrating Generic Sensor Fusion Algorithms with
    // Sound State Representation through Encapsulation of Manifolds"
    //
    // https://arxiv.org/pdf/1107.1119.pdf

    double norm = gcem::hypot(m_x, m_y, m_z);

    double scalar;
    if (norm < 1e-9) {
      scalar = (2.0 / W() - 2.0 / 3.0 * norm * norm / (W() * W() * W()));
    } else {
      if (W() < 0.0) {
        scalar = 2.0 * gcem::atan2(-norm, -W()) / norm;
      } else {
        scalar = 2.0 * gcem::atan2(norm, W()) / norm;
      }
    }

    return Eigen::Vector3d{{scalar * m_x, scalar * m_y, scalar * m_z}};
  }

  /**
   * Returns the quaternion representation of this rotation vector.
   *
   * This is also the exp operator of 𝖘𝖔(3).
   *
   * source: wpimath/algorithms.md
   */
  constexpr static Quaternion FromRotationVector(const Eigen::Vector3d& rvec) {
    // 𝑣⃗ = θ * v̂
    // v̂ = 𝑣⃗ / θ

    // 𝑞 = std::cos(θ/2) + std::sin(θ/2) * v̂
    // 𝑞 = std::cos(θ/2) + std::sin(θ/2) / θ * 𝑣⃗

    double theta = gcem::hypot(rvec.coeff(0), rvec.coeff(1), rvec.coeff(2));
    double cos = gcem::cos(theta / 2);

    double axial_scalar;

    if (theta < 1e-9) {
      // taylor series expansion of sin(θ/2) / θ around θ = 0 = 1/2 - θ²/48 +
      // O(θ⁴)
      axial_scalar = 1.0 / 2.0 - theta * theta / 48.0;
    } else {
      axial_scalar = gcem::sin(theta / 2) / theta;
    }

    return Quaternion{cos, axial_scalar * rvec.coeff(0),
                      axial_scalar * rvec.coeff(1),
                      axial_scalar * rvec.coeff(2)};
  }

 private:
  // Scalar r in versor form
  double m_w = 1.0;

  // Vector v in versor form
  double m_x = 0.0;
  double m_y = 0.0;
  double m_z = 0.0;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Quaternion& quaternion);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Quaternion& quaternion);

}  // namespace frc

#ifndef NO_PROTOBUF
#include "frc/geometry/proto/QuaternionProto.h"
#endif
#include "frc/geometry/struct/QuaternionStruct.h"
