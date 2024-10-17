// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <type_traits>

#include <Eigen/Core>
#include <Eigen/LU>
#include <fmt/format.h>
#include <gcem.hpp>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/ct_matrix.h"
#include "frc/fmt/Eigen.h"
#include "frc/geometry/Quaternion.h"
#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"
#include "units/math.h"
#include "wpimath/MathShared.h"

namespace frc {

/**
 * A rotation in a 3D coordinate frame represented by a quaternion.
 */
class WPILIB_DLLEXPORT Rotation3d {
 public:
  /**
   * Constructs a Rotation3d representing no rotation.
   */
  constexpr Rotation3d() = default;

  /**
   * Constructs a Rotation3d from a quaternion.
   *
   * @param q The quaternion.
   */
  constexpr explicit Rotation3d(const Quaternion& q) { m_q = q.Normalize(); }

  /**
   * Constructs a Rotation3d from extrinsic roll, pitch, and yaw.
   *
   * Extrinsic rotations occur in that order around the axes in the fixed global
   * frame rather than the body frame.
   *
   * Angles are measured counterclockwise with the rotation axis pointing "out
   * of the page". If you point your right thumb along the positive axis
   * direction, your fingers curl in the direction of positive rotation.
   *
   * @param roll The counterclockwise rotation angle around the X axis (roll).
   * @param pitch The counterclockwise rotation angle around the Y axis (pitch).
   * @param yaw The counterclockwise rotation angle around the Z axis (yaw).
   */
  constexpr Rotation3d(units::radian_t roll, units::radian_t pitch,
                       units::radian_t yaw) {
    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_angles_to_quaternion_conversion
    double cr = units::math::cos(roll * 0.5);
    double sr = units::math::sin(roll * 0.5);

    double cp = units::math::cos(pitch * 0.5);
    double sp = units::math::sin(pitch * 0.5);

    double cy = units::math::cos(yaw * 0.5);
    double sy = units::math::sin(yaw * 0.5);

    m_q = Quaternion{cr * cp * cy + sr * sp * sy, sr * cp * cy - cr * sp * sy,
                     cr * sp * cy + sr * cp * sy, cr * cp * sy - sr * sp * cy};
  }

  /**
   * Constructs a Rotation3d with the given axis-angle representation. The axis
   * doesn't have to be normalized.
   *
   * @param axis The rotation axis.
   * @param angle The rotation around the axis.
   */
  constexpr Rotation3d(const Eigen::Vector3d& axis, units::radian_t angle) {
    double norm = ct_matrix{axis}.norm();
    if (norm == 0.0) {
      return;
    }

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Definition
    Eigen::Vector3d v{{axis.coeff(0) / norm * units::math::sin(angle / 2.0),
                       axis.coeff(1) / norm * units::math::sin(angle / 2.0),
                       axis.coeff(2) / norm * units::math::sin(angle / 2.0)}};
    m_q = Quaternion{units::math::cos(angle / 2.0), v.coeff(0), v.coeff(1),
                     v.coeff(2)};
  }

  /**
   * Constructs a Rotation3d with the given rotation vector representation. This
   * representation is equivalent to axis-angle, where the normalized axis is
   * multiplied by the rotation around the axis in radians.
   *
   * @param rvec The rotation vector.
   */
  constexpr explicit Rotation3d(const Eigen::Vector3d& rvec)
      : Rotation3d{rvec, units::radian_t{ct_matrix{rvec}.norm()}} {}

  /**
   * Constructs a Rotation3d from a rotation matrix.
   *
   * @param rotationMatrix The rotation matrix.
   * @throws std::domain_error if the rotation matrix isn't special orthogonal.
   */
  constexpr explicit Rotation3d(const Eigen::Matrix3d& rotationMatrix) {
    auto impl = []<typename Matrix3d>(const Matrix3d& R) -> Quaternion {
      // Require that the rotation matrix is special orthogonal. This is true if
      // the matrix is orthogonal (RRᵀ = I) and normalized (determinant is 1).
      if ((R * R.transpose() - Matrix3d::Identity()).norm() > 1e-9) {
        throw std::domain_error("Rotation matrix isn't orthogonal");
      }
      if (gcem::abs(R.determinant() - 1.0) > 1e-9) {
        throw std::domain_error(
            "Rotation matrix is orthogonal but not special orthogonal");
      }

      // Turn rotation matrix into a quaternion
      // https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
      double trace = R(0, 0) + R(1, 1) + R(2, 2);
      double w;
      double x;
      double y;
      double z;

      if (trace > 0.0) {
        double s = 0.5 / gcem::sqrt(trace + 1.0);
        w = 0.25 / s;
        x = (R(2, 1) - R(1, 2)) * s;
        y = (R(0, 2) - R(2, 0)) * s;
        z = (R(1, 0) - R(0, 1)) * s;
      } else {
        if (R(0, 0) > R(1, 1) && R(0, 0) > R(2, 2)) {
          double s = 2.0 * gcem::sqrt(1.0 + R(0, 0) - R(1, 1) - R(2, 2));
          w = (R(2, 1) - R(1, 2)) / s;
          x = 0.25 * s;
          y = (R(0, 1) + R(1, 0)) / s;
          z = (R(0, 2) + R(2, 0)) / s;
        } else if (R(1, 1) > R(2, 2)) {
          double s = 2.0 * gcem::sqrt(1.0 + R(1, 1) - R(0, 0) - R(2, 2));
          w = (R(0, 2) - R(2, 0)) / s;
          x = (R(0, 1) + R(1, 0)) / s;
          y = 0.25 * s;
          z = (R(1, 2) + R(2, 1)) / s;
        } else {
          double s = 2.0 * gcem::sqrt(1.0 + R(2, 2) - R(0, 0) - R(1, 1));
          w = (R(1, 0) - R(0, 1)) / s;
          x = (R(0, 2) + R(2, 0)) / s;
          y = (R(1, 2) + R(2, 1)) / s;
          z = 0.25 * s;
        }
      }

      return Quaternion{w, x, y, z};
    };

    if (std::is_constant_evaluated()) {
      m_q = impl(ct_matrix3d{rotationMatrix});
    } else {
      m_q = impl(rotationMatrix);
    }
  }

  /**
   * Constructs a Rotation3d that rotates the initial vector onto the final
   * vector.
   *
   * This is useful for turning a 3D vector (final) into an orientation relative
   * to a coordinate system vector (initial).
   *
   * @param initial The initial vector.
   * @param final The final vector.
   */
  constexpr Rotation3d(const Eigen::Vector3d& initial,
                       const Eigen::Vector3d& final) {
    double dot = ct_matrix{initial}.dot(ct_matrix{final});
    double normProduct = ct_matrix{initial}.norm() * ct_matrix{final}.norm();
    double dotNorm = dot / normProduct;

    if (dotNorm > 1.0 - 1E-9) {
      // If the dot product is 1, the two vectors point in the same direction so
      // there's no rotation. The default initialization of m_q will work.
      return;
    } else if (dotNorm < -1.0 + 1E-9) {
      // If the dot product is -1, the two vectors are antiparallel, so a 180°
      // rotation is required. Any other vector can be used to generate an
      // orthogonal one.

      double x = gcem::abs(initial.coeff(0));
      double y = gcem::abs(initial.coeff(1));
      double z = gcem::abs(initial.coeff(2));

      // Find vector that is most orthogonal to initial vector
      Eigen::Vector3d other;
      if (x < y) {
        if (x < z) {
          // Use x-axis
          other = Eigen::Vector3d{{1, 0, 0}};
        } else {
          // Use z-axis
          other = Eigen::Vector3d{{0, 0, 1}};
        }
      } else {
        if (y < z) {
          // Use y-axis
          other = Eigen::Vector3d{{0, 1, 0}};
        } else {
          // Use z-axis
          other = Eigen::Vector3d{{0, 0, 1}};
        }
      }

      auto axis = ct_matrix{initial}.cross(ct_matrix{other});

      double axisNorm = axis.norm();
      m_q = Quaternion{0.0, axis(0) / axisNorm, axis(1) / axisNorm,
                       axis(2) / axisNorm};
    } else {
      auto axis = ct_matrix{initial}.cross(final);

      // https://stackoverflow.com/a/11741520
      m_q =
          Quaternion{normProduct + dot, axis(0), axis(1), axis(2)}.Normalize();
    }
  }

  /**
   * Adds two rotations together.
   *
   * @param other The rotation to add.
   *
   * @return The sum of the two rotations.
   */
  constexpr Rotation3d operator+(const Rotation3d& other) const {
    return RotateBy(other);
  }

  /**
   * Subtracts the new rotation from the current rotation and returns the new
   * rotation.
   *
   * @param other The rotation to subtract.
   *
   * @return The difference between the two rotations.
   */
  constexpr Rotation3d operator-(const Rotation3d& other) const {
    return *this + -other;
  }

  /**
   * Takes the inverse of the current rotation.
   *
   * @return The inverse of the current rotation.
   */
  constexpr Rotation3d operator-() const { return Rotation3d{m_q.Inverse()}; }

  /**
   * Multiplies the current rotation by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Rotation3d.
   */
  constexpr Rotation3d operator*(double scalar) const {
    // https://en.wikipedia.org/wiki/Slerp#Quaternion_Slerp
    if (m_q.W() >= 0.0) {
      return Rotation3d{Eigen::Vector3d{{m_q.X(), m_q.Y(), m_q.Z()}},
                        2.0 * units::radian_t{scalar * gcem::acos(m_q.W())}};
    } else {
      return Rotation3d{Eigen::Vector3d{{-m_q.X(), -m_q.Y(), -m_q.Z()}},
                        2.0 * units::radian_t{scalar * gcem::acos(-m_q.W())}};
    }
  }

  /**
   * Divides the current rotation by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Rotation3d.
   */
  constexpr Rotation3d operator/(double scalar) const {
    return *this * (1.0 / scalar);
  }

  /**
   * Checks equality between this Rotation3d and another object.
   */
  constexpr bool operator==(const Rotation3d& other) const {
    return gcem::abs(gcem::abs(m_q.Dot(other.m_q)) -
                     m_q.Norm() * other.m_q.Norm()) < 1e-9;
  }

  /**
   * Adds the new rotation to the current rotation. The other rotation is
   * applied extrinsically, which means that it rotates around the global axes.
   * For example, Rotation3d{90_deg, 0, 0}.RotateBy(Rotation3d{0, 45_deg, 0})
   * rotates by 90 degrees around the +X axis and then by 45 degrees around the
   * global +Y axis. (This is equivalent to Rotation3d{90_deg, 45_deg, 0})
   *
   * @param other The extrinsic rotation to rotate by.
   *
   * @return The new rotated Rotation3d.
   */
  constexpr Rotation3d RotateBy(const Rotation3d& other) const {
    return Rotation3d{other.m_q * m_q};
  }

  /**
   * Returns the quaternion representation of the Rotation3d.
   */
  constexpr const Quaternion& GetQuaternion() const { return m_q; }

  /**
   * Returns the counterclockwise rotation angle around the X axis (roll).
   */
  constexpr units::radian_t X() const {
    double w = m_q.W();
    double x = m_q.X();
    double y = m_q.Y();
    double z = m_q.Z();

    // wpimath/algorithms.md
    double cxcy = 1.0 - 2.0 * (x * x + y * y);
    double sxcy = 2.0 * (w * x + y * z);
    double cy_sq = cxcy * cxcy + sxcy * sxcy;
    if (cy_sq > 1e-20) {
      return units::radian_t{gcem::atan2(sxcy, cxcy)};
    } else {
      return 0_rad;
    }
  }

  /**
   * Returns the counterclockwise rotation angle around the Y axis (pitch).
   */
  constexpr units::radian_t Y() const {
    double w = m_q.W();
    double x = m_q.X();
    double y = m_q.Y();
    double z = m_q.Z();

    // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_(in_3-2-1_sequence)_conversion
    double ratio = 2.0 * (w * y - z * x);
    if (gcem::abs(ratio) >= 1.0) {
      return units::radian_t{gcem::copysign(std::numbers::pi / 2.0, ratio)};
    } else {
      return units::radian_t{gcem::asin(ratio)};
    }
  }

  /**
   * Returns the counterclockwise rotation angle around the Z axis (yaw).
   */
  constexpr units::radian_t Z() const {
    double w = m_q.W();
    double x = m_q.X();
    double y = m_q.Y();
    double z = m_q.Z();

    // wpimath/algorithms.md
    double cycz = 1.0 - 2.0 * (y * y + z * z);
    double cysz = 2.0 * (w * z + x * y);
    double cy_sq = cycz * cycz + cysz * cysz;
    if (cy_sq > 1e-20) {
      return units::radian_t{gcem::atan2(cysz, cycz)};
    } else {
      return units::radian_t{gcem::atan2(2.0 * w * z, w * w - z * z)};
    }
  }

  /**
   * Returns the axis in the axis-angle representation of this rotation.
   */
  constexpr Eigen::Vector3d Axis() const {
    double norm = gcem::hypot(m_q.X(), m_q.Y(), m_q.Z());
    if (norm == 0.0) {
      return Eigen::Vector3d{{0.0, 0.0, 0.0}};
    } else {
      return Eigen::Vector3d{{m_q.X() / norm, m_q.Y() / norm, m_q.Z() / norm}};
    }
  }

  /**
   * Returns the angle in the axis-angle representation of this rotation.
   */
  constexpr units::radian_t Angle() const {
    double norm = gcem::hypot(m_q.X(), m_q.Y(), m_q.Z());
    return units::radian_t{2.0 * gcem::atan2(norm, m_q.W())};
  }

  /**
   * Returns a Rotation2d representing this Rotation3d projected into the X-Y
   * plane.
   */
  constexpr Rotation2d ToRotation2d() const { return Rotation2d{Z()}; }

 private:
  Quaternion m_q;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Rotation3d& rotation);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Rotation3d& rotation);

}  // namespace frc

#ifndef NO_PROTOBUF
#include "frc/geometry/proto/Rotation3dProto.h"
#endif
#include "frc/geometry/struct/Rotation3dStruct.h"
