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

class Pose2d;

class Transform2d {
 public:
  Transform2d(Pose2d initial, Pose2d final);
  Transform2d(Translation2d translation, Rotation2d rotation);
  Transform2d();

  Transform2d Inverse() const;

  const Translation2d& Translation() const { return m_translation; }
  const Rotation2d& Rotation() const { return m_rotation; }

 private:
  Translation2d m_translation;
  Rotation2d m_rotation;
};
}  // namespace experimental
}  // namespace frc