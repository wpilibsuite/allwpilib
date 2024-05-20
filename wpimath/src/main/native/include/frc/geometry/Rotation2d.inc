// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <gcem.hpp>

#include "frc/geometry/Rotation2d.h"
#include "units/angle.h"

namespace frc {

constexpr Rotation2d::Rotation2d(units::angle_unit auto value)
    : m_value{value},
      m_cos{gcem::cos(value.template convert<units::radian>().value())},
      m_sin{gcem::sin(value.template convert<units::radian>().value())} {}

constexpr Rotation2d::Rotation2d(double x, double y) {
  double magnitude = gcem::hypot(x, y);
  if (magnitude > 1e-6) {
    m_sin = y / magnitude;
    m_cos = x / magnitude;
  } else {
    m_sin = 0.0;
    m_cos = 1.0;
  }
  m_value = units::radian_t{gcem::atan2(m_sin, m_cos)};
}

constexpr Rotation2d Rotation2d::operator-() const {
  return Rotation2d{-m_value};
}

constexpr Rotation2d Rotation2d::operator*(double scalar) const {
  return Rotation2d{m_value * scalar};
}

constexpr Rotation2d Rotation2d::operator+(const Rotation2d& other) const {
  return RotateBy(other);
}

constexpr Rotation2d Rotation2d::operator-(const Rotation2d& other) const {
  return *this + -other;
}

constexpr Rotation2d Rotation2d::operator/(double scalar) const {
  return *this * (1.0 / scalar);
}

constexpr bool Rotation2d::operator==(const Rotation2d& other) const {
  return gcem::hypot(Cos() - other.Cos(), Sin() - other.Sin()) < 1E-9;
}

constexpr Rotation2d Rotation2d::RotateBy(const Rotation2d& other) const {
  return {Cos() * other.Cos() - Sin() * other.Sin(),
          Cos() * other.Sin() + Sin() * other.Cos()};
}

}  // namespace frc
