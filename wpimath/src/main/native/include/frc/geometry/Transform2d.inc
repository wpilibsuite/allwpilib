// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Translation2d.h"

namespace frc {

constexpr Transform2d::Transform2d(Translation2d translation,
                                   Rotation2d rotation)
    : m_translation{std::move(translation)}, m_rotation{std::move(rotation)} {}

constexpr Transform2d::Transform2d(units::meter_t x, units::meter_t y,
                                   Rotation2d rotation)
    : m_translation{x, y}, m_rotation{std::move(rotation)} {}

constexpr Transform2d Transform2d::Inverse() const {
  // We are rotating the difference between the translations
  // using a clockwise rotation matrix. This transforms the global
  // delta into a local delta (relative to the initial pose).
  return Transform2d{(-Translation()).RotateBy(-Rotation()), -Rotation()};
}

}  // namespace frc
