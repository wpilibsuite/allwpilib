// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <initializer_list>
#include <span>
#include <utility>

#include <gcem.hpp>
#include <wpi/SymbolExports.h>
#include <wpi/json_fwd.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/geometry/Twist2d.h"
#include "units/length.h"

namespace frc {

class Transform2d;

/**
 * Represents a 2D pose containing translational and rotational elements.
 */
class WPILIB_DLLEXPORT Pose2d {
 public:
  /**
   * Constructs a pose at the origin facing toward the positive X axis.
   */
  constexpr Pose2d() = default;

  /**
   * Constructs a pose with the specified translation and rotation.
   *
   * @param translation The translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  constexpr Pose2d(Translation2d translation, Rotation2d rotation)
      : m_translation{std::move(translation)},
        m_rotation{std::move(rotation)} {}

  /**
   * Constructs a pose with x and y translations instead of a separate
   * Translation2d.
   *
   * @param x The x component of the translational component of the pose.
   * @param y The y component of the translational component of the pose.
   * @param rotation The rotational component of the pose.
   */
  constexpr Pose2d(units::meter_t x, units::meter_t y, Rotation2d rotation)
      : m_translation{x, y}, m_rotation{std::move(rotation)} {}

  /**
   * Constructs a pose with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws std::domain_error if the affine transformation matrix is invalid.
   */
  constexpr explicit Pose2d(const Eigen::Matrix3d& matrix)
      : m_translation{Eigen::Vector2d{{matrix(0, 2)}, {matrix(1, 2)}}},
        m_rotation{Eigen::Matrix2d{{matrix(0, 0), matrix(0, 1)},
                                   {matrix(1, 0), matrix(1, 1)}}} {
    if (matrix(2, 0) != 0.0 || matrix(2, 1) != 0.0 || matrix(2, 2) != 1.0) {
      throw std::domain_error("Affine transformation matrix is invalid");
    }
  }

  /**
   * Transforms the pose by the given transformation and returns the new
   * transformed pose.
   *
   * <pre>
   * [x_new]    [cos, -sin, 0][transform.x]
   * [y_new] += [sin,  cos, 0][transform.y]
   * [t_new]    [  0,    0, 1][transform.t]
   * </pre>
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  constexpr Pose2d operator+(const Transform2d& other) const {
    return TransformBy(other);
  }

  /**
   * Returns the Transform2d that maps the one pose to another.
   *
   * @param other The initial pose of the transformation.
   * @return The transform that maps the other pose to the current pose.
   */
  constexpr Transform2d operator-(const Pose2d& other) const;

  /**
   * Checks equality between this Pose2d and another object.
   */
  constexpr bool operator==(const Pose2d&) const = default;

  /**
   * Returns the underlying translation.
   *
   * @return Reference to the translational component of the pose.
   */
  constexpr const Translation2d& Translation() const { return m_translation; }

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
   * Returns the underlying rotation.
   *
   * @return Reference to the rotational component of the pose.
   */
  constexpr const Rotation2d& Rotation() const { return m_rotation; }

  /**
   * Multiplies the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  constexpr Pose2d operator*(double scalar) const {
    return Pose2d{m_translation * scalar, m_rotation * scalar};
  }

  /**
   * Divides the current pose by a scalar.
   *
   * @param scalar The scalar.
   *
   * @return The new scaled Pose2d.
   */
  constexpr Pose2d operator/(double scalar) const {
    return *this * (1.0 / scalar);
  }

  /**
   * Rotates the pose around the origin and returns the new pose.
   *
   * @param other The rotation to transform the pose by.
   *
   * @return The rotated pose.
   */
  constexpr Pose2d RotateBy(const Rotation2d& other) const {
    return {m_translation.RotateBy(other), m_rotation.RotateBy(other)};
  }

  /**
   * Transforms the pose by the given transformation and returns the new pose.
   * See + operator for the matrix multiplication performed.
   *
   * @param other The transform to transform the pose by.
   *
   * @return The transformed pose.
   */
  constexpr Pose2d TransformBy(const Transform2d& other) const;

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
  constexpr Pose2d RelativeTo(const Pose2d& other) const;

  /**
   * Rotates the current pose around a point in 2D space.
   *
   * @param point The point in 2D space to rotate around.
   * @param rot The rotation to rotate the pose by.
   *
   * @return The new rotated pose.
   */
  constexpr Pose2d RotateAround(const Translation2d& point,
                                const Rotation2d& rot) const {
    return {m_translation.RotateAround(point, rot), m_rotation.RotateBy(rot)};
  }

  /**
   * Obtain a new Pose2d from a (constant curvature) velocity.
   *
   * See https://file.tavsys.net/control/controls-engineering-in-frc.pdf section
   * 10.2 "Pose exponential" for a derivation.
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
   * update, the twist would be Twist2d{0.01_m, 0_m, 0.5_deg}.
   *
   * @return The new pose of the robot.
   */
  constexpr Pose2d Exp(const Twist2d& twist) const;

  /**
   * Returns a Twist2d that maps this pose to the end pose. If c is the output
   * of a.Log(b), then a.Exp(c) would yield b.
   *
   * @param end The end pose for the transformation.
   *
   * @return The twist that maps this to end.
   */
  constexpr Twist2d Log(const Pose2d& end) const;

  /**
   * Returns an affine transformation matrix representation of this pose.
   */
  constexpr Eigen::Matrix3d ToMatrix() const {
    auto vec = m_translation.ToVector();
    auto mat = m_rotation.ToMatrix();
    return Eigen::Matrix3d{{mat(0, 0), mat(0, 1), vec(0)},
                           {mat(1, 0), mat(1, 1), vec(1)},
                           {0.0, 0.0, 1.0}};
  }

  /**
   * Returns the nearest Pose2d from a collection of poses
   * @param poses The collection of poses.
   * @return The nearest Pose2d from the collection.
   */
  constexpr Pose2d Nearest(std::span<const Pose2d> poses) const {
    return *std::min_element(
        poses.begin(), poses.end(), [this](const Pose2d& a, const Pose2d& b) {
          auto aDistance = this->Translation().Distance(a.Translation());
          auto bDistance = this->Translation().Distance(b.Translation());

          // If the distances are equal sort by difference in rotation
          if (aDistance == bDistance) {
            return gcem::abs(
                       (this->Rotation() - a.Rotation()).Radians().value()) <
                   gcem::abs(
                       (this->Rotation() - b.Rotation()).Radians().value());
          }
          return aDistance < bDistance;
        });
  }

  /**
   * Returns the nearest Pose2d from a collection of poses
   * @param poses The collection of poses.
   * @return The nearest Pose2d from the collection.
   */
  constexpr Pose2d Nearest(std::initializer_list<Pose2d> poses) const {
    return *std::min_element(
        poses.begin(), poses.end(), [this](const Pose2d& a, const Pose2d& b) {
          auto aDistance = this->Translation().Distance(a.Translation());
          auto bDistance = this->Translation().Distance(b.Translation());

          // If the distances are equal sort by difference in rotation
          if (aDistance == bDistance) {
            return gcem::abs(
                       (this->Rotation() - a.Rotation()).Radians().value()) <
                   gcem::abs(
                       (this->Rotation() - b.Rotation()).Radians().value());
          }
          return aDistance < bDistance;
        });
  }

 private:
  Translation2d m_translation;
  Rotation2d m_rotation;
};

WPILIB_DLLEXPORT
void to_json(wpi::json& json, const Pose2d& pose);

WPILIB_DLLEXPORT
void from_json(const wpi::json& json, Pose2d& pose);

}  // namespace frc

#include "frc/geometry/proto/Pose2dProto.h"
#include "frc/geometry/struct/Pose2dStruct.h"

#include "frc/geometry/Transform2d.h"

namespace frc {

constexpr Transform2d Pose2d::operator-(const Pose2d& other) const {
  const auto pose = this->RelativeTo(other);
  return Transform2d{pose.Translation(), pose.Rotation()};
}

constexpr Pose2d Pose2d::TransformBy(const frc::Transform2d& other) const {
  return {m_translation + (other.Translation().RotateBy(m_rotation)),
          other.Rotation() + m_rotation};
}

constexpr Pose2d Pose2d::RelativeTo(const Pose2d& other) const {
  const Transform2d transform{other, *this};
  return {transform.Translation(), transform.Rotation()};
}

constexpr Pose2d Pose2d::Exp(const Twist2d& twist) const {
  const auto dx = twist.dx;
  const auto dy = twist.dy;
  const auto dtheta = twist.dtheta.value();

  const auto sinTheta = gcem::sin(dtheta);
  const auto cosTheta = gcem::cos(dtheta);

  double s, c;
  if (gcem::abs(dtheta) < 1E-9) {
    s = 1.0 - 1.0 / 6.0 * dtheta * dtheta;
    c = 0.5 * dtheta;
  } else {
    s = sinTheta / dtheta;
    c = (1 - cosTheta) / dtheta;
  }

  const Transform2d transform{Translation2d{dx * s - dy * c, dx * c + dy * s},
                              Rotation2d{cosTheta, sinTheta}};

  return *this + transform;
}

constexpr Twist2d Pose2d::Log(const Pose2d& end) const {
  const auto transform = end.RelativeTo(*this);
  const auto dtheta = transform.Rotation().Radians().value();
  const auto halfDtheta = dtheta / 2.0;

  const auto cosMinusOne = transform.Rotation().Cos() - 1;

  double halfThetaByTanOfHalfDtheta;

  if (gcem::abs(cosMinusOne) < 1E-9) {
    halfThetaByTanOfHalfDtheta = 1.0 - 1.0 / 12.0 * dtheta * dtheta;
  } else {
    halfThetaByTanOfHalfDtheta =
        -(halfDtheta * transform.Rotation().Sin()) / cosMinusOne;
  }

  const Translation2d translationPart =
      transform.Translation().RotateBy(
          {halfThetaByTanOfHalfDtheta, -halfDtheta}) *
      gcem::hypot(halfThetaByTanOfHalfDtheta, halfDtheta);

  return {translationPart.X(), translationPart.Y(), units::radian_t{dtheta}};
}

}  // namespace frc
