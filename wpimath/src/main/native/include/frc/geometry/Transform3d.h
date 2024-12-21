// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include <wpi/SymbolExports.h>

#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Translation3d.h"

namespace frc {

class Pose3d;

/**
 * Represents a transformation for a Pose3d in the pose's frame.
 */
class WPILIB_DLLEXPORT Transform3d {
 public:
  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param final The final pose for the transformation.
   */
  constexpr Transform3d(const Pose3d& initial, const Pose3d& final);

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  constexpr Transform3d(Translation3d translation, Rotation3d rotation)
      : m_translation{std::move(translation)},
        m_rotation{std::move(rotation)} {}

  /**
   * Constructs a transform with x, y, and z translations instead of a separate
   * Translation3d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param z The z component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  constexpr Transform3d(units::meter_t x, units::meter_t y, units::meter_t z,
                        Rotation3d rotation)
      : m_translation{x, y, z}, m_rotation{std::move(rotation)} {}

  /**
   * Constructs a transform with the specified affine transformation matrix.
   *
   * @param matrix The affine transformation matrix.
   * @throws std::domain_error if the affine transformation matrix is invalid.
   */
  constexpr explicit Transform3d(const Eigen::Matrix4d& matrix)
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
   * Constructs the identity transform -- maps an initial pose to itself.
   */
  constexpr Transform3d() = default;

  /**
   * Constructs a 3D transform from a 2D transform in the X-Y plane.
   **
   * @param transform The 2D transform.
   * @see Rotation3d(Rotation2d)
   * @see Translation3d(Translation2d)
   */
  constexpr explicit Transform3d(const frc::Transform2d& transform)
      : m_translation{frc::Translation3d{transform.Translation()}},
        m_rotation{frc::Rotation3d{transform.Rotation()}} {}

  /**
   * Returns the translation component of the transformation.
   *
   * @return Reference to the translational component of the transform.
   */
  constexpr const Translation3d& Translation() const { return m_translation; }

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
   * Returns the Z component of the transformation's translation.
   *
   * @return The z component of the transformation's translation.
   */
  constexpr units::meter_t Z() const { return m_translation.Z(); }

  /**
   * Returns an affine transformation matrix representation of this
   * transformation.
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
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  constexpr const Rotation3d& Rotation() const { return m_rotation; }

  /**
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  constexpr Transform3d Inverse() const {
    // We are rotating the difference between the translations
    // using a clockwise rotation matrix. This transforms the global
    // delta into a local delta (relative to the initial pose).
    return Transform3d{(-Translation()).RotateBy(-Rotation()), -Rotation()};
  }

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  constexpr Transform3d operator*(double scalar) const {
    return Transform3d(m_translation * scalar, m_rotation * scalar);
  }

  /**
   * Divides the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  constexpr Transform3d operator/(double scalar) const {
    return *this * (1.0 / scalar);
  }

  /**
   * Composes two transformations. The second transform is applied relative to
   * the orientation of the first.
   *
   * @param other The transform to compose with this one.
   * @return The composition of the two transformations.
   */
  constexpr Transform3d operator+(const Transform3d& other) const;

  /**
   * Checks equality between this Transform3d and another object.
   */
  constexpr bool operator==(const Transform3d&) const = default;

 private:
  Translation3d m_translation;
  Rotation3d m_rotation;
};

}  // namespace frc

#include "frc/geometry/Pose3d.h"

namespace frc {

constexpr Transform3d::Transform3d(const Pose3d& initial, const Pose3d& final) {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  m_translation = (final.Translation() - initial.Translation())
                      .RotateBy(-initial.Rotation());

  m_rotation = final.Rotation() - initial.Rotation();
}

constexpr Transform3d Transform3d::operator+(const Transform3d& other) const {
  return Transform3d{Pose3d{}, Pose3d{}.TransformBy(*this).TransformBy(other)};
}

}  // namespace frc

#include "frc/geometry/proto/Transform3dProto.h"
#include "frc/geometry/struct/Transform3dStruct.h"
