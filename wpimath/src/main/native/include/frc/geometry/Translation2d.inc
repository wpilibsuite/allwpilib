// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/geometry/Translation2d.h"
#include "units/length.h"

namespace frc {

constexpr Translation2d::Translation2d(units::meter_t x, units::meter_t y)
    : m_x{x}, m_y{y} {}

constexpr Translation2d::Translation2d(units::meter_t distance,
                                       const Rotation2d& angle)
    : m_x{distance * angle.Cos()}, m_y{distance * angle.Sin()} {}

constexpr Eigen::Vector2d Translation2d::ToVector() const {
  return Eigen::Vector2d{{m_x.value(), m_y.value()}};
}

constexpr Rotation2d Translation2d::Angle() const {
  return Rotation2d{m_x.value(), m_y.value()};
}

constexpr Translation2d Translation2d::RotateBy(const Rotation2d& other) const {
  return {m_x * other.Cos() - m_y * other.Sin(),
          m_x * other.Sin() + m_y * other.Cos()};
}

constexpr Translation2d Translation2d::operator+(
    const Translation2d& other) const {
  return {X() + other.X(), Y() + other.Y()};
}

constexpr Translation2d Translation2d::operator-(
    const Translation2d& other) const {
  return *this + -other;
}

constexpr Translation2d Translation2d::operator-() const {
  return {-m_x, -m_y};
}

constexpr Translation2d Translation2d::operator*(double scalar) const {
  return {scalar * m_x, scalar * m_y};
}

constexpr Translation2d Translation2d::operator/(double scalar) const {
  return operator*(1.0 / scalar);
}

}  // namespace frc
