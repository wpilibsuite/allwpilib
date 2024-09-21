// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Transform2d.h"

#include "frc/geometry/Pose2d.h"

using namespace frc;

Transform2d::Transform2d(Pose2d initial, Pose2d final) {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  m_translation = (final.Translation() - initial.Translation())
                      .RotateBy(-initial.Rotation());

  m_rotation = final.Rotation() - initial.Rotation();
}

Transform2d Transform2d::operator+(const Transform2d& other) const {
  return Transform2d{Pose2d{}, Pose2d{}.TransformBy(*this).TransformBy(other)};
}
