// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "units/length.h"

namespace frc {

constexpr Pose2d::Pose2d(Translation2d translation, Rotation2d rotation)
    : m_translation{std::move(translation)}, m_rotation{std::move(rotation)} {}

constexpr Pose2d::Pose2d(units::meter_t x, units::meter_t y,
                         Rotation2d rotation)
    : m_translation{x, y}, m_rotation{std::move(rotation)} {}

constexpr Pose2d Pose2d::operator+(const Transform2d& other) const {
  return TransformBy(other);
}

constexpr Pose2d Pose2d::operator*(double scalar) const {
  return Pose2d{m_translation * scalar, m_rotation * scalar};
}

constexpr Pose2d Pose2d::operator/(double scalar) const {
  return *this * (1.0 / scalar);
}

constexpr Pose2d Pose2d::RotateBy(const Rotation2d& other) const {
  return {m_translation.RotateBy(other), m_rotation.RotateBy(other)};
}

constexpr Pose2d Pose2d::TransformBy(const Transform2d& other) const {
  return {m_translation + (other.Translation().RotateBy(m_rotation)),
          other.Rotation() + m_rotation};
}

}  // namespace frc
