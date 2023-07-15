// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Transform3d.h"

#include "frc/geometry/Pose3d.h"

using namespace frc;

Transform3d::Transform3d(Pose3d initial, Pose3d final) {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  m_translation = (final.Translation() - initial.Translation())
                      .RotateBy(-initial.Rotation());

  m_rotation = final.Rotation() - initial.Rotation();
}

Transform3d::Transform3d(Translation3d translation, Rotation3d rotation)
    : m_translation(std::move(translation)), m_rotation(std::move(rotation)) {}

Transform3d Transform3d::Inverse() const {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  return Transform3d{(-Translation()).RotateBy(-Rotation()), -Rotation()};
}

Transform3d Transform3d::operator+(const Transform3d& other) const {
  return Transform3d{Pose3d{}, Pose3d{}.TransformBy(*this).TransformBy(other)};
}
