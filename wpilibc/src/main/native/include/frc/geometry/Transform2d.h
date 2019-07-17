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
   */
  Transform2d(Pose2d initial, Pose2d final);

  /**
   * Constructs a transform with the given translation and rotation components.
   */
  Transform2d(Translation2d translation, Rotation2d rotation);

  /**
   * Constructs the identity transform -- maps an initial pose to itself.
   */
  Transform2d();

  /**
   * Returns the translation component of the transformation.
   */
  const Translation2d& Translation() const { return m_translation; }

  /**
   * Returns the rotational component of the transformation.
   */
  const Rotation2d& Rotation() const { return m_rotation; }

 private:
  Translation2d m_translation;
  Rotation2d m_rotation;
};
}  // namespace frc