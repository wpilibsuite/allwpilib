/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

Transform2d::Transform2d(Translation2d translation, Rotation2d rotation)
    : m_translation(translation), m_rotation(rotation) {}

Transform2d Transform2d::Inverse() const {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  return Transform2d{(-Translation()).RotateBy(-Rotation()), -Rotation()};
}

bool Transform2d::operator==(const Transform2d& other) const {
  return m_translation == other.m_translation && m_rotation == other.m_rotation;
}

bool Transform2d::operator!=(const Transform2d& other) const {
  return !operator==(other);
}
