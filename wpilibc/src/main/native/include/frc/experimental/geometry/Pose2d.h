/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include "Translation2d.h"

namespace frc {
namespace experimental {
class Pose2d {
 public:
  Pose2d();
  Pose2d(Translation2d translation, Rotation2d rotation);
  Pose2d(double x, double y, Rotation2d rotation);

  Pose2d operator+(const Pose2d& other) const;
  void operator+=(const Pose2d& other);

  const Translation2d& Translation() const { return m_translation; }
  const Rotation2d& Rotation() const { return m_rotation; }

  // TODO figure out if this should be mutating or not.
  Pose2d TransformBy(const Pose2d& other) const;

  Pose2d Inverse() const;
  Pose2d RelativeTo(const Pose2d& other) const;

 private:
  Translation2d m_translation;
  Rotation2d m_rotation;
};
}  // namespace experimental
}  // namespace frc
