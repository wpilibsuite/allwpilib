// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Translation2d.h"

#include <wpi/json.h>

#include "units/math.h"

using namespace frc;

Translation2d::Translation2d(units::meter_t x, units::meter_t y)
    : m_x(x), m_y(y) {}

Translation2d::Translation2d(units::meter_t distance, const Rotation2d& angle)
    : m_x(distance * angle.Cos()), m_y(distance * angle.Sin()) {}

units::meter_t Translation2d::Distance(const Translation2d& other) const {
  return units::math::hypot(other.m_x - m_x, other.m_y - m_y);
}

units::meter_t Translation2d::Norm() const {
  return units::math::hypot(m_x, m_y);
}

Translation2d Translation2d::RotateBy(const Rotation2d& other) const {
  return {m_x * other.Cos() - m_y * other.Sin(),
          m_x * other.Sin() + m_y * other.Cos()};
}

Translation2d Translation2d::operator+(const Translation2d& other) const {
  return {X() + other.X(), Y() + other.Y()};
}

Translation2d& Translation2d::operator+=(const Translation2d& other) {
  m_x += other.m_x;
  m_y += other.m_y;
  return *this;
}

Translation2d Translation2d::operator-(const Translation2d& other) const {
  return *this + -other;
}

Translation2d& Translation2d::operator-=(const Translation2d& other) {
  *this += -other;
  return *this;
}

Translation2d Translation2d::operator-() const {
  return {-m_x, -m_y};
}

Translation2d Translation2d::operator*(double scalar) const {
  return {scalar * m_x, scalar * m_y};
}

Translation2d& Translation2d::operator*=(double scalar) {
  m_x *= scalar;
  m_y *= scalar;
  return *this;
}

Translation2d Translation2d::operator/(double scalar) const {
  return *this * (1.0 / scalar);
}

bool Translation2d::operator==(const Translation2d& other) const {
  return units::math::abs(m_x - other.m_x) < 1E-9_m &&
         units::math::abs(m_y - other.m_y) < 1E-9_m;
}

bool Translation2d::operator!=(const Translation2d& other) const {
  return !operator==(other);
}

Translation2d& Translation2d::operator/=(double scalar) {
  *this *= (1.0 / scalar);
  return *this;
}

void frc::to_json(wpi::json& json, const Translation2d& translation) {
  json = wpi::json{{"x", translation.X().to<double>()},
                   {"y", translation.Y().to<double>()}};
}

void frc::from_json(const wpi::json& json, Translation2d& translation) {
  translation = Translation2d{units::meter_t{json.at("x").get<double>()},
                              units::meter_t{json.at("y").get<double>()}};
}
