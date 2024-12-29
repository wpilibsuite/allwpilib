// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include <wpi/SymbolExports.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"

namespace frc {

class Pose2d;

/**
 * Represents a transformation for a Pose2d in the pose's frame.
 */
class WPILIB_DLLEXPORT Transform2d {
 public:
  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param final The final pose for the transformation.
   */
  constexpr Transform2d(const Pose2d& initial, const Pose2d& final);

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  constexpr Transform2d(Translation2d translation, Rotation2d rotation)
      : m_translation{std::move(translation)},
        m_rotation{std::move(rotation)} {}

  /**
   * Constructs a transform with x and y translations instead of a separate
   * Translation2d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  constexpr Transform2d(units::meter_t x, units::meter_t y, Rotation2d rotation)
      : m_translation{x, y}, m_rotation{std::move(rotation)} {}

  /**
   * Constructs a pose with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws std::domain_error if the affine transformation matrix is invalid.
   */
  constexpr explicit Transform2d(const Eigen::Matrix3d& matrix)
      : m_translation{Eigen::Vector2d{{matrix(0, 2)}, {matrix(1, 2)}}},
        m_rotation{Eigen::Matrix2d{{matrix(0, 0), matrix(0, 1)},
                                   {matrix(1, 0), matrix(1, 1)}}} {
    if (matrix(2, 0) != 0.0 || matrix(2, 1) != 0.0 || matrix(2, 2) != 1.0) {
      throw std::domain_error("Affine transformation matrix is invalid");
    }
  }

  /**
   * Constructs the identity transform -- maps an initial pose to itself.
   */
  constexpr Transform2d() = default;

  /**
   * Returns the translation component of the transformation.
   *
   * @return Reference to the translational component of the transform.
   */
  constexpr const Translation2d& Translation() const { return m_translation; }

  /**
   * Returns the X component of the transformation's translation.
   *
   * @return The x component of the transformation's translation.
   */
  constexpr units::meter_t X() const { return m_translation.X(); }

  /**
   * Returns the Y component of the transformation's translation.
   *
   * @return The y component of the transformation's translation.
   */
  constexpr units::meter_t Y() const { return m_translation.Y(); }

  /**
   * Returns an affine transformation matrix representation of this
   * transformation.
   */
  constexpr Eigen::Matrix3d ToMatrix() const {
    auto vec = m_translation.ToVector();
    auto mat = m_rotation.ToMatrix();
    return Eigen::Matrix3d{{mat(0, 0), mat(0, 1), vec(0)},
                           {mat(1, 0), mat(1, 1), vec(1)},
                           {0.0, 0.0, 1.0}};
  }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  constexpr const Rotation2d& Rotation() const { return m_rotation; }

  /**
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  constexpr Transform2d Inverse() const {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    return Transform2d{(-Translation()).RotateBy(-Rotation()), -Rotation()};
  }

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  constexpr Transform2d operator*(double scalar) const {
    return Transform2d(m_translation * scalar, m_rotation * scalar);
  }

  /**
   * Divides the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  constexpr Transform2d operator/(double scalar) const {
    return *this * (1.0 / scalar);
  }

  /**
   * Composes two transformations. The second transform is applied relative to
   * the orientation of the first.
   *
   * @param other The transform to compose with this one.
   * @return The composition of the two transformations.
   */
  constexpr Transform2d operator+(const Transform2d& other) const;

  /**
   * Checks equality between this Transform2d and another object.
   */
  constexpr bool operator==(const Transform2d&) const = default;

 private:
  Translation2d m_translation;
  Rotation2d m_rotation;
};

}  // namespace frc

#include "frc/geometry/Pose2d.h"

namespace frc {

constexpr Transform2d::Transform2d(const Pose2d& initial, const Pose2d& final) {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  m_translation = (final.Translation() - initial.Translation())
                      .RotateBy(-initial.Rotation());

  m_rotation = final.Rotation() - initial.Rotation();
}

constexpr Transform2d Transform2d::operator+(const Transform2d& other) const {
  return Transform2d{Pose2d{}, Pose2d{}.TransformBy(*this).TransformBy(other)};
}

}  // namespace frc

#include "frc/geometry/proto/Transform2dProto.h"
#include "frc/geometry/struct/Transform2dStruct.h"
