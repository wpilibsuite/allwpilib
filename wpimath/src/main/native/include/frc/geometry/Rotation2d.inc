// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>

#include "frc/geometry/Rotation2d.h"
#include "gcem.hpp"
#include "units/angle.h"

namespace frc {

constexpr Rotation2d::Rotation2d(units::radian_t value)
    : m_value(value),
      m_cos(std::is_constant_evaluated() ? gcem::cos(value.to<double>())
                                         : std::cos(value.to<double>())),
      m_sin(std::is_constant_evaluated() ? gcem::sin(value.to<double>())
                                         : std::sin(value.to<double>())) {}

constexpr Rotation2d::Rotation2d(units::degree_t value)
    : Rotation2d(units::radian_t{value}) {}

constexpr Rotation2d::Rotation2d(double x, double y) {
  const auto magnitude = gcem::hypot(x, y);
  if (magnitude > 1e-6) {
    m_sin = y / magnitude;
    m_cos = x / magnitude;
  } else {
    m_sin = 0.0;
    m_cos = 1.0;
  }
  m_value =
      units::radian_t{std::is_constant_evaluated() ? gcem::atan2(m_sin, m_cos)
                                                   : std::atan2(m_sin, m_cos)};
}

constexpr Rotation2d Rotation2d::operator-() const {
  return Rotation2d{-m_value};
}

constexpr Rotation2d Rotation2d::operator*(double scalar) const {
  return Rotation2d(m_value * scalar);
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
  return (std::is_constant_evaluated()
              ? gcem::hypot(Cos() - other.Cos(), Sin() - other.Sin())
              : std::hypot(Cos() - other.Cos(), Sin() - other.Sin())) < 1E-9;
}

constexpr Rotation2d Rotation2d::RotateBy(const Rotation2d& other) const {
  return {Cos() * other.Cos() - Sin() * other.Sin(),
          Cos() * other.Sin() + Sin() * other.Cos()};
}

}  // namespace frc
