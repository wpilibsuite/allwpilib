// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <wpi/SymbolExports.h>

#include "frc/geometry/Translation3d.h"

namespace frc {

class WPILIB_DLLEXPORT Pose3d;

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
  Transform3d(Pose3d initial, Pose3d final);

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  Transform3d(Translation3d translation, Rotation3d rotation);

  /**
   * Constructs a transform with x, y, and z translations instead of a separate
   * Translation3d.
   *
   * @param x The x component of the translational component of the transform.
   * @param y The y component of the translational component of the transform.
   * @param z The z component of the translational component of the transform.
   * @param rotation The rotational component of the transform.
   */
  Transform3d(units::meter_t x, units::meter_t y, units::meter_t z,
              Rotation3d rotation);

  /**
   * Constructs the identity transform -- maps an initial pose to itself.
   */
  constexpr Transform3d() = default;

  /**
   * Returns the translation component of the transformation.
   *
   * @return Reference to the translational component of the transform.
   */
  const Translation3d& Translation() const { return m_translation; }

  /**
   * Returns the X component of the transformation's translation.
   *
   * @return The x component of the transformation's translation.
   */
  units::meter_t X() const { return m_translation.X(); }

  /**
   * Returns the Y component of the transformation's translation.
   *
   * @return The y component of the transformation's translation.
   */
  units::meter_t Y() const { return m_translation.Y(); }

  /**
   * Returns the Z component of the transformation's translation.
   *
   * @return The z component of the transformation's translation.
   */
  units::meter_t Z() const { return m_translation.Z(); }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  const Rotation3d& Rotation() const { return m_rotation; }

  /**
   * Invert the transformation. This is useful for undoing a transformation.
   *
   * @return The inverted transformation.
   */
  Transform3d Inverse() const;

  /**
   * Multiplies the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  Transform3d operator*(double scalar) const {
    return Transform3d(m_translation * scalar, m_rotation * scalar);
  }

  /**
   * Divides the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform3d.
   */
  Transform3d operator/(double scalar) const { return *this * (1.0 / scalar); }

  /**
   * Composes two transformations. The second transform is applied relative to
   * the orientation of the first.
   *
   * @param other The transform to compose with this one.
   * @return The composition of the two transformations.
   */
  Transform3d operator+(const Transform3d& other) const;

  /**
   * Checks equality between this Transform3d and another object.
   */
  bool operator==(const Transform3d&) const = default;

 private:
  Translation3d m_translation;
  Rotation3d m_rotation;
};
}  // namespace frc

#include "frc/geometry/proto/Transform3dProto.h"
#include "frc/geometry/struct/Transform3dStruct.h"
