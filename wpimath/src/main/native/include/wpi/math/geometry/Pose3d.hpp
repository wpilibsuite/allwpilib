// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <initializer_list>
#include <span>
#include <stdexcept>
#include <utility>

#include <Eigen/Core>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/json_fwd.hpp"

namespace wpi::math {

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
  constexpr Pose3d(wpi::units::meter_t x, wpi::units::meter_t y,
                   wpi::units::meter_t z, Rotation3d rotation)
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
  constexpr wpi::units::meter_t X() const { return m_translation.X(); }

  /**
   * Returns the Y component of the pose's translation.
   *
   * @return The y component of the pose's translation.
   */
  constexpr wpi::units::meter_t Y() const { return m_translation.Y(); }

  /**
   * Returns the Z component of the pose's translation.
   *
   * @return The z component of the pose's translation.
   */
  constexpr wpi::units::meter_t Z() const { return m_translation.Z(); }

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

  /**
   * Returns the nearest Pose3d from a collection of poses.
   *
   * If two or more poses in the collection have the same distance from this
   * pose, return the one with the closest rotation component.
   *
   * @param poses The collection of poses.
   * @return The nearest Pose3d from the collection.
   */
  constexpr Pose3d Nearest(std::span<const Pose3d> poses) const {
    return *std::min_element(
        poses.begin(), poses.end(), [this](const Pose3d& a, const Pose3d& b) {
          auto aDistance = this->Translation().Distance(a.Translation());
          auto bDistance = this->Translation().Distance(b.Translation());

          // If the distances are equal sort by difference in rotation
          if (aDistance == bDistance) {
            return gcem::abs(this->Rotation()
                                 .RelativeTo(a.Rotation())
                                 .Angle()
                                 .value()) <
                   gcem::abs(this->Rotation()
                                 .RelativeTo(b.Rotation())
                                 .Angle()
                                 .value());
          }
          return aDistance < bDistance;
        });
  }

  /**
   * Returns the nearest Pose3d from a collection of poses.
   *
   * If two or more poses in the collection have the same distance from this
   * pose, return the one with the closest rotation component.
   *
   * @param poses The collection of poses.
   * @return The nearest Pose3d from the collection.
   */
  constexpr Pose3d Nearest(std::initializer_list<Pose3d> poses) const {
    return *std::min_element(
        poses.begin(), poses.end(), [this](const Pose3d& a, const Pose3d& b) {
          auto aDistance = this->Translation().Distance(a.Translation());
          auto bDistance = this->Translation().Distance(b.Translation());

          // If the distances are equal sort by difference in rotation
          if (aDistance == bDistance) {
            return gcem::abs(this->Rotation()
                                 .RelativeTo(a.Rotation())
                                 .Angle()
                                 .value()) <
                   gcem::abs(this->Rotation()
                                 .RelativeTo(b.Rotation())
                                 .Angle()
                                 .value());
          }
          return aDistance < bDistance;
        });
  }

 private:
  Translation3d m_translation;
  Rotation3d m_rotation;
};

WPILIB_DLLEXPORT
void to_json(wpi::util::json& json, const Pose3d& pose);

WPILIB_DLLEXPORT
void from_json(const wpi::util::json& json, Pose3d& pose);

}  // namespace wpi::math

#include "wpi/math/geometry/Transform3d.hpp"

namespace wpi::math {

constexpr Transform3d Pose3d::operator-(const Pose3d& other) const {
  const auto pose = this->RelativeTo(other);
  return Transform3d{pose.Translation(), pose.Rotation()};
}

constexpr Pose3d Pose3d::TransformBy(const Transform3d& other) const {
  return {m_translation + other.Translation().RotateBy(m_rotation),
          other.Rotation().RotateBy(m_rotation)};
}

constexpr Pose3d Pose3d::RelativeTo(const Pose3d& other) const {
  const Transform3d transform{other, *this};
  return {transform.Translation(), transform.Rotation()};
}

}  // namespace wpi::math

#include "wpi/math/geometry/proto/Pose3dProto.hpp"
#include "wpi/math/geometry/struct/Pose3dStruct.hpp"
