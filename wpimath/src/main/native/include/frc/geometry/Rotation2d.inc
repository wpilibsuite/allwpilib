// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"

namespace frc {

constexpr Rotation2d::Rotation2d(units::radian_t value) : m_value(value)
                                                        , m_cos(std::cos(value.to<double>()))
                                                        , m_sin(std::sin(value.to<double>())) {}

constexpr Rotation2d::Rotation2d(units::degree_t value) : m_value(value)
                                                        , m_cos(std::cos(units::radian_t{value}.to<double>()))
                                                        , m_sin(std::sin(units::radian_t{value}.to<double>())) {}

constexpr Rotation2d Rotation2d::operator-() const {
  return Rotation2d(-m_value);
}

constexpr Rotation2d Rotation2d::operator*(double scalar) const {
  return Rotation2d(m_value * scalar);
}

}  // namespace frc
