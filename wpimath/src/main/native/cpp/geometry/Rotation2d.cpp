// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Rotation2d.h"

#include <cmath>

#include <wpi/json.h>

#include "units/math.h"

using namespace frc;

Rotation2d::Rotation2d(double x, double y) {
  const auto magnitude = std::hypot(x, y);
  if (magnitude > 1e-6) {
    m_sin = y / magnitude;
    m_cos = x / magnitude;
  } else {
    m_sin = 0.0;
    m_cos = 1.0;
  }
  m_value = units::radian_t{std::atan2(m_sin, m_cos)};
}

Rotation2d Rotation2d::operator+(const Rotation2d& other) const {
  return RotateBy(other);
}

Rotation2d Rotation2d::operator-(const Rotation2d& other) const {
  return *this + -other;
}

Rotation2d Rotation2d::operator-() const {
  return Rotation2d{-m_value};
}

Rotation2d Rotation2d::operator*(double scalar) const {
  return Rotation2d{m_value * scalar};
}

Rotation2d Rotation2d::operator/(double scalar) const {
  return *this * (1.0 / scalar);
}

bool Rotation2d::operator==(const Rotation2d& other) const {
  return std::hypot(Cos() - other.Cos(), Sin() - other.Sin()) < 1E-9;
}

bool Rotation2d::operator!=(const Rotation2d& other) const {
  return !operator==(other);
}

Rotation2d Rotation2d::RotateBy(const Rotation2d& other) const {
  return {Cos() * other.Cos() - Sin() * other.Sin(),
          Cos() * other.Sin() + Sin() * other.Cos()};
}

double Rotation2d::Cos() const {
  return m_cos;
}

double Rotation2d::Sin() const {
  return m_sin;
}

void frc::to_json(wpi::json& json, const Rotation2d& rotation) {
  json = wpi::json{{"radians", rotation.Radians().value()}};
}

void frc::from_json(const wpi::json& json, Rotation2d& rotation) {
  rotation = Rotation2d{units::radian_t{json.at("radians").get<double>()}};
}
