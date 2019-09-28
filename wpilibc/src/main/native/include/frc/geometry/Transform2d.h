/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Translation2d.h"

namespace frc {

class Pose2d;

/**
 * Represents a transformation for a Pose2d.
 */
class Transform2d {
 public:
  /**
   * Constructs the transform that maps the initial pose to the final pose.
   *
   * @param initial The initial pose for the transformation.
   * @param final The final pose for the transformation.
   */
  Transform2d(Pose2d initial, Pose2d final);

  /**
   * Constructs a transform with the given translation and rotation components.
   *
   * @param translation Translational component of the transform.
   * @param rotation Rotational component of the transform.
   */
  Transform2d(Translation2d translation, Rotation2d rotation);

  /**
   * Constructs the identity transform -- maps an initial pose to itself.
   */
  constexpr Transform2d() = default;

  /**
   * Returns the translation component of the transformation.
   *
   * @return Reference to the translational component of the transform.
   */
  const Translation2d& Translation() const { return m_translation; }

  /**
   * Returns the rotational component of the transformation.
   *
   * @return Reference to the rotational component of the transform.
   */
  const Rotation2d& Rotation() const { return m_rotation; }

  /**
   * Scales the transform by the scalar.
   *
   * @param scalar The scalar.
   * @return The scaled Transform2d.
   */
  Transform2d operator*(double scalar) const {
    return Transform2d(m_translation * scalar, m_rotation * scalar);
  }

  /**
   * Checks equality between this Transform2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are equal.
   */
  bool operator==(const Transform2d& other) const;

  /**
   * Checks inequality between this Transform2d and another object.
   *
   * @param other The other object.
   * @return Whether the two objects are not equal.
   */
  bool operator!=(const Transform2d& other) const;

 private:
  Translation2d m_translation;
  Rotation2d m_rotation;
};
}  // namespace frc
