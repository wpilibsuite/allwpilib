/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/geometry/Pose2d.h"

using namespace frc::experimental;

Pose2d::Pose2d() : m_translation(Translation2d()), m_rotation(Rotation2d()) {}

Pose2d::Pose2d(const Translation2d translation, const Rotation2d rotation)
    : m_translation(translation), m_rotation(rotation) {}

Pose2d::Pose2d(const double x, const double y, const Rotation2d rotation)
    : m_translation(Translation2d(x, y)), m_rotation(rotation) {}

Pose2d Pose2d::operator+(const Pose2d& other) const {
  return TransformBy(other);
}

void Pose2d::operator+=(const Pose2d& other) {
  m_translation += other.m_translation.RotateBy(m_rotation);
  m_rotation += other.m_rotation;
}

Pose2d Pose2d::TransformBy(const Pose2d& other) const {
  return {m_translation + (other.m_translation.RotateBy(m_rotation)),
          m_rotation + other.m_rotation};
}

Pose2d Pose2d::Inverse() const {
  return {(-m_translation).RotateBy(-m_rotation), -m_rotation};
}

Pose2d Pose2d::RelativeTo(const Pose2d& other) const {
  return other.Inverse() + *this;
}
