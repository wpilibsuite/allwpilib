// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdexcept>
#include <type_traits>
#include <utility>

#include <Eigen/Core>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/ct_matrix.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation3d.h"
#include "frc/geometry/Translation3d.h"
#include "frc/geometry/Twist3d.h"

namespace frc {

class Transform3d;

/**
 * Represents a 3D pose containing translational and rotational elements.
 */
class WPILIB_DLLEXPORT Pose3d {
 public:
  /**
   * Constructs a pose at the origin facing toward the positive X axis.
   */
  constexpr Pose3d() = default;

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  constexpr Pose3d(Translation3d translation, Rotation3d rotation)
      : m_translation{std::move(translation)},
        m_rotation{std::move(rotation)} {}

  /**
   * Constructs a pose with x, y, and z translations instead of a separate
   * Translation3d.
   *
   * @param x The x component of the translational component of the pose.
   * @param y The y component of the translational component of the pose.
   * @param z The z component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  constexpr Pose3d(units::meter_t x, units::meter_t y, units::meter_t z,
                   Rotation3d rotation)
      : m_translation{x, y, z}, m_rotation{std::move(rotation)} {}

  /**
   * Constructs a pose with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws std::domain_error if the affine transformation matrix is invalid.
   */
  constexpr explicit Pose3d(const Eigen::Matrix4d& matrix)
      : m_translation{Eigen::Vector3d{
            {matrix(0, 3)}, {matrix(1, 3)}, {matrix(2, 3)}}},
        m_rotation{
            Eigen::Matrix3d{{matrix(0, 0), matrix(0, 1), matrix(0, 2)},
                            {matrix(1, 0), matrix(1, 1), matrix(1, 2)},
                            {matrix(2, 0), matrix(2, 1), matrix(2, 2)}}} {
    if (matrix(3, 0) != 0.0 || matrix(3, 1) != 0.0 || matrix(3, 2) != 0.0 ||
        matrix(3, 3) != 1.0) {
      throw std::domain_error("Affine transformation matrix is invalid");
    }
  }

  /**
   * Constructs a 3D pose from a 2D pose in the X-Y plane.
   *
   * @param pose The 2D pose.
   * @see Rotation3d(Rotation2d)
   * @see Translation3d(Translation2d)
   */
  constexpr explicit Pose3d(const Pose2d& pose)
      : m_translation{pose.X(), pose.Y(), 0_m},
        m_rotation{0_rad, 0_rad, pose.Rotation().Radians()} {}

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose. The transform is applied relative to the pose's frame.
   * Note that this differs from Pose3d::RotateBy(const Rotation3d&), which is
   * applied relative to the global frame and around the origin.
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  constexpr Pose3d operator+(const Transform3d& other) const {
    return TransformBy(other);
  }

  /**
   * Returns the Transform3d that maps the one pose to another.
   *
   * @param other The initial pose of the transformation.
   * @return The transform that maps the other pose to the current pose.
   */
  constexpr Transform3d operator-(const Pose3d& other) const;

  /**
   * Checks equality between this Pose3d and another object.
   */
  constexpr bool operator==(const Pose3d&) const = default;

  /**
   * Returns the underlying translation.
   *
   * @return Reference to the translational component of the pose.
   */
  constexpr const Translation3d& Translation() const { return m_translation; }

  /**
   * Returns the X component of the pose's translation.
   *
   * @return The x component of the pose's translation.
   */
  constexpr units::meter_t X() const { return m_translation.X(); }

  /**
   * Returns the Y component of the pose's translation.
   *
   * @return The y component of the pose's translation.
   */
  constexpr units::meter_t Y() const { return m_translation.Y(); }

  /**
   * Returns the Z component of the pose's translation.
   *
   * @return The z component of the pose's translation.
   */
  constexpr units::meter_t Z() const { return m_translation.Z(); }

  /**
   * Returns the underlying rotation.
   *
   * @return Reference to the rotational component of the pose.
   */
  constexpr const Rotation3d& Rotation() const { return m_rotation; }

  /**
   * Multiplies the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  constexpr Pose3d operator*(double scalar) const {
    return Pose3d{m_translation * scalar, m_rotation * scalar};
  }

  /**
   * Divides the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  constexpr Pose3d operator/(double scalar) const {
    return *this * (1.0 / scalar);
  }

  /**
   * Rotates the pose around the origin and returns the new pose.
   *
   * @param other The rotation to transform the pose by, which is applied
   * extrinsically (from the global frame).
   *
   * @return The rotated pose.
   */
  constexpr Pose3d RotateBy(const Rotation3d& other) const {
    return {m_translation.RotateBy(other), m_rotation.RotateBy(other)};
  }

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose. The transform is applied relative to the pose's frame.
   * Note that this differs from Pose3d::RotateBy(const Rotation3d&), which is
   * applied relative to the global frame and around the origin.
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  constexpr Pose3d TransformBy(const Transform3d& other) const;

  /**
   * Returns the current pose relative to the given pose.
   *
   * This function can often be used for trajectory tracking or pose
   * stabilization algorithms to get the error between the reference and the
   * current pose.
   *
   * @param other The pose that is the origin of the new coordinate frame that
   * the current pose will be converted into.
   *
   * @return The current pose relative to the new origin pose.
   */
  constexpr Pose3d RelativeTo(const Pose3d& other) const;

  /**
   * Rotates the current pose around a point in 3D space.
   *
   * @param point The point in 3D space to rotate around.
   * @param rot The rotation to rotate the pose by.
   *
   * @return The new rotated pose.
   */
  constexpr Pose3d RotateAround(const Translation3d& point,
                                const Rotation3d& rot) const {
    return {m_translation.RotateAround(point, rot), m_rotation.RotateBy(rot)};
  }

  /**
   * Obtain a new Pose3d from a (constant curvature) velocity.
   *
   * The twist is a change in pose in the robot's coordinate frame since the
   * previous pose update. When the user runs exp() on the previous known
   * field-relative pose with the argument being the twist, the user will
   * receive the new field-relative pose.
   *
   * "Exp" represents the pose exponential, which is solving a differential
   * equation moving the pose forward in time.
   *
   * @param twist The change in pose in the robot's coordinate frame since the
   * previous pose update. For example, if a non-holonomic robot moves forward
   * 0.01 meters and changes angle by 0.5 degrees since the previous pose
   * update, the twist would be Twist3d{0.01_m, 0_m, 0_m, Rotation3d{0.0, 0.0,
   * 0.5_deg}}.
   *
   * @return The new pose of the robot.
   */
  constexpr Pose3d Exp(const Twist3d& twist) const;

  /**
   * Returns a Twist3d that maps this pose to the end pose. If c is the output
   * of a.Log(b), then a.Exp(c) would yield b.
   *
   * @param end The end pose for the transformation.
   *
   * @return The twist that maps this to end.
   */
  constexpr Twist3d Log(const Pose3d& end) const;

  /**
   * Returns an affine transformation matrix representation of this pose.
   */
  constexpr Eigen::Matrix4d ToMatrix() const {
    auto vec = m_translation.ToVector();
    auto mat = m_rotation.ToMatrix();
    return Eigen::Matrix4d{{mat(0, 0), mat(0, 1), mat(0, 2), vec(0)},
                           {mat(1, 0), mat(1, 1), mat(1, 2), vec(1)},
                           {mat(2, 0), mat(2, 1), mat(2, 2), vec(2)},
                           {0.0, 0.0, 0.0, 1.0}};
  }

  /**
   * Returns a Pose2d representing this Pose3d projected into the X-Y plane.
   */
  constexpr Pose2d ToPose2d() const {
    return Pose2d{m_translation.X(), m_translation.Y(), m_rotation.Z()};
  }

 private:
  Translation3d m_translation;
  Rotation3d m_rotation;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Pose3d& pose);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Pose3d& pose);

}  // namespace frc

#include "frc/geometry/proto/Pose3dProto.h"
#include "frc/geometry/struct/Pose3dStruct.h"

#include "frc/geometry/Transform3d.h"

namespace frc {

namespace detail {

/**
 * Applies the hat operator to a rotation vector.
 *
 * It takes a rotation vector and returns the corresponding matrix
 * representation of the Lie algebra element (a 3x3 rotation matrix).
 *
 * @param rotation The rotation vector.
 * @return The rotation vector as a 3x3 rotation matrix.
 */
constexpr ct_matrix3d RotationVectorToMatrix(const ct_vector3d& rotation) {
  // Given a rotation vector <a, b, c>,
  //         [ 0 -c  b]
  // Omega = [ c  0 -a]
  //         [-b  a  0]
  return ct_matrix3d{{0.0, -rotation(2), rotation(1)},
                     {rotation(2), 0.0, -rotation(0)},
                     {-rotation(1), rotation(0), 0.0}};
}

/**
 * Applies the hat operator to a rotation vector.
 *
 * It takes a rotation vector and returns the corresponding matrix
 * representation of the Lie algebra element (a 3x3 rotation matrix).
 *
 * @param rotation The rotation vector.
 * @return The rotation vector as a 3x3 rotation matrix.
 */
constexpr Eigen::Matrix3d RotationVectorToMatrix(
    const Eigen::Vector3d& rotation) {
  // Given a rotation vector <a, b, c>,
  //         [ 0 -c  b]
  // Omega = [ c  0 -a]
  //         [-b  a  0]
  return Eigen::Matrix3d{{0.0, -rotation(2), rotation(1)},
                         {rotation(2), 0.0, -rotation(0)},
                         {-rotation(1), rotation(0), 0.0}};
}

}  // namespace detail

constexpr Transform3d Pose3d::operator-(const Pose3d& other) const {
  const auto pose = this->RelativeTo(other);
  return Transform3d{pose.Translation(), pose.Rotation()};
}

constexpr Pose3d Pose3d::TransformBy(const Transform3d& other) const {
  return {m_translation + (other.Translation().RotateBy(m_rotation)),
          other.Rotation() + m_rotation};
}

constexpr Pose3d Pose3d::RelativeTo(const Pose3d& other) const {
  const Transform3d transform{other, *this};
  return {transform.Translation(), transform.Rotation()};
}

constexpr Pose3d Pose3d::Exp(const Twist3d& twist) const {
  // Implementation from Section 3.2 of https://ethaneade.org/lie.pdf

  auto impl = [this]<typename Matrix3d, typename Vector3d>(
                  const Twist3d& twist) -> Pose3d {
    Vector3d u{{twist.dx.value(), twist.dy.value(), twist.dz.value()}};
    Vector3d rvec{{twist.rx.value(), twist.ry.value(), twist.rz.value()}};
    Matrix3d omega = detail::RotationVectorToMatrix(rvec);
    Matrix3d omegaSq = omega * omega;
    double theta = rvec.norm();
    double thetaSq = theta * theta;

    double A;
    double B;
    double C;
    if (gcem::abs(theta) < 1E-7) {
      // Taylor Expansions around θ = 0
      // A = 1/1! - θ²/3! + θ⁴/5!
      // B = 1/2! - θ²/4! + θ⁴/6!
      // C = 1/3! - θ²/5! + θ⁴/7!
      // sources:
      // A:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D+at+x%3D0
      // B:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      // C:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      A = 1 - thetaSq / 6 + thetaSq * thetaSq / 120;
      B = 1 / 2.0 - thetaSq / 24 + thetaSq * thetaSq / 720;
      C = 1 / 6.0 - thetaSq / 120 + thetaSq * thetaSq / 5040;
    } else {
      // A = std::sin(θ)/θ
      // B = (1 - std::cos(θ)) / θ²
      // C = (1 - A) / θ²
      A = gcem::sin(theta) / theta;
      B = (1 - gcem::cos(theta)) / thetaSq;
      C = (1 - A) / thetaSq;
    }

    Matrix3d R = Matrix3d::Identity() + A * omega + B * omegaSq;
    Matrix3d V = Matrix3d::Identity() + B * omega + C * omegaSq;

    Vector3d translation_component = V * u;

    const Transform3d transform{
        Translation3d{units::meter_t{translation_component(0)},
                      units::meter_t{translation_component(1)},
                      units::meter_t{translation_component(2)}},
        Rotation3d{R}};

    return *this + transform;
  };

  if (std::is_constant_evaluated()) {
    return impl.template operator()<ct_matrix3d, ct_vector3d>(twist);
  } else {
    return impl.template operator()<Eigen::Matrix3d, Eigen::Vector3d>(twist);
  }
}

constexpr Twist3d Pose3d::Log(const Pose3d& end) const {
  // Implementation from Section 3.2 of https://ethaneade.org/lie.pdf

  auto impl = [this]<typename Matrix3d, typename Vector3d>(
                  const Pose3d& end) -> Twist3d {
    const auto transform = end.RelativeTo(*this);

    Vector3d u{
        {transform.X().value(), transform.Y().value(), transform.Z().value()}};
    Vector3d rvec = transform.Rotation().ToVector();
    Matrix3d omega = detail::RotationVectorToMatrix(rvec);
    Matrix3d omegaSq = omega * omega;
    double theta = rvec.norm();
    double thetaSq = theta * theta;

    double C;
    if (gcem::abs(theta) < 1E-7) {
      // Taylor Expansions around θ = 0
      // A = 1/1! - θ²/3! + θ⁴/5!
      // B = 1/2! - θ²/4! + θ⁴/6!
      // C = 1/6 * (1/2 + θ²/5! + θ⁴/7!)
      // sources:
      // A:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D+at+x%3D0
      // B:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      // C:
      // https://www.wolframalpha.com/input?i2d=true&i=series+expansion+of+Divide%5B1-Divide%5BDivide%5Bsin%5C%2840%29x%5C%2841%29%2Cx%5D%2C2Divide%5B1-cos%5C%2840%29x%5C%2841%29%2CPower%5Bx%2C2%5D%5D%5D%2CPower%5Bx%2C2%5D%5D+at+x%3D0
      C = 1 / 12.0 + thetaSq / 720 + thetaSq * thetaSq / 30240;
    } else {
      // A = std::sin(θ)/θ
      // B = (1 - std::cos(θ)) / θ²
      // C = (1 - A/(2*B)) / θ²
      double A = gcem::sin(theta) / theta;
      double B = (1 - gcem::cos(theta)) / thetaSq;
      C = (1 - A / (2 * B)) / thetaSq;
    }

    Matrix3d V_inv = Matrix3d::Identity() - 0.5 * omega + C * omegaSq;

    Vector3d translation_component = V_inv * u;

    return Twist3d{units::meter_t{translation_component(0)},
                   units::meter_t{translation_component(1)},
                   units::meter_t{translation_component(2)},
                   units::radian_t{rvec(0)},
                   units::radian_t{rvec(1)},
                   units::radian_t{rvec(2)}};
  };

  if (std::is_constant_evaluated()) {
    return impl.template operator()<ct_matrix3d, ct_vector3d>(end);
  } else {
    return impl.template operator()<Eigen::Matrix3d, Eigen::Vector3d>(end);
  }
}

}  // namespace frc
