/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/geometry/Rotation2d.h"

#include <cmath>

#include <wpi/json.h>

#include "units/math.h"

using namespace frc;

Rotation2d::Rotation2d(units::radian_t value)
    : m_value(value),
      m_cos(units::math::cos(value)),
      m_sin(units::math::sin(value)) {}

Rotation2d::Rotation2d(units::degree_t value)
    : m_value(value),
      m_cos(units::math::cos(value)),
      m_sin(units::math::sin(value)) {}

Rotation2d::Rotation2d(double x, double y) {
  const auto magnitude = std::hypot(x, y);
  if (magnitude > 1e-6) {
    m_sin = y / magnitude;
    m_cos = x / magnitude;
  } else {
    m_sin = 0.0;
    m_cos = 1.0;
  }
  m_value = units::radian_t(std::atan2(m_sin, m_cos));
}

Rotation2d Rotation2d::operator+(const Rotation2d& other) const {
  return RotateBy(other);
}

Rotation2d& Rotation2d::operator+=(const Rotation2d& other) {
  double cos = Cos() * other.Cos() - Sin() * other.Sin();
  double sin = Cos() * other.Sin() + Sin() * other.Cos();
  m_cos = cos;
  m_sin = sin;
  m_value = units::radian_t(std::atan2(m_sin, m_cos));
  return *this;
}

Rotation2d Rotation2d::operator-(const Rotation2d& other) const {
  return *this + -other;
}

Rotation2d& Rotation2d::operator-=(const Rotation2d& other) {
  *this += -other;
  return *this;
}

Rotation2d Rotation2d::operator-() const { return Rotation2d(-m_value); }

Rotation2d Rotation2d::operator*(double scalar) const {
  return Rotation2d(m_value * scalar);
}

bool Rotation2d::operator==(const Rotation2d& other) const {
  return units::math::abs(m_value - other.m_value) < 1E-9_rad;
}

bool Rotation2d::operator!=(const Rotation2d& other) const {
  return !operator==(other);
}

Rotation2d Rotation2d::RotateBy(const Rotation2d& other) const {
  return {Cos() * other.Cos() - Sin() * other.Sin(),
          Cos() * other.Sin() + Sin() * other.Cos()};
}

void frc::to_json(wpi::json& json, const Rotation2d& rotation) {
  json = wpi::json{{"radians", rotation.Radians().to<double>()}};
}

void frc::from_json(const wpi::json& json, Rotation2d& rotation) {
  rotation = Rotation2d{units::radian_t{json.at("radians").get<double>()}};
}
