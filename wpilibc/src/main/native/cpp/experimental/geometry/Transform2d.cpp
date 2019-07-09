/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/geometry/Transform2d.h"
#include "frc/experimental/geometry/Pose2d.h"

using namespace frc::experimental;

Transform2d::Transform2d(const Pose2d initial, const Pose2d final) {
  m_translation = (final.Translation() - initial.Translation())
                      .RotateBy(-initial.Rotation());

  m_rotation = final.Rotation() - initial.Rotation();
}

Transform2d::Transform2d(Translation2d translation, Rotation2d rotation)
    : m_translation(translation), m_rotation(rotation) {}

Transform2d::Transform2d()
    : m_translation(Translation2d()), m_rotation(Rotation2d()) {}
