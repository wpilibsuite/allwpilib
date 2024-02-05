// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "frc/geometry/Translation2d.h"
#include "frc/geometry/Translation3d.h"
#include "units/length.h"
#include "units/math.h"

namespace frc {

constexpr Translation3d::Translation3d(units::meter_t x, units::meter_t y,
                                       units::meter_t z)
    : m_x{x}, m_y{y}, m_z{z} {}

constexpr Translation2d Translation3d::ToTranslation2d() const {
  return Translation2d{m_x, m_y};
}

constexpr Eigen::Vector3d Translation3d::ToVector() const {
  return Eigen::Vector3d{{m_x.value(), m_y.value(), m_z.value()}};
}

constexpr Translation3d Translation3d::operator+(
    const Translation3d& other) const {
  return {X() + other.X(), Y() + other.Y(), Z() + other.Z()};
}

constexpr Translation3d Translation3d::operator-(
    const Translation3d& other) const {
  return operator+(-other);
}

constexpr Translation3d Translation3d::operator-() const {
  return {-m_x, -m_y, -m_z};
}

constexpr Translation3d Translation3d::operator*(double scalar) const {
  return {scalar * m_x, scalar * m_y, scalar * m_z};
}

constexpr Translation3d Translation3d::operator/(double scalar) const {
  return operator*(1.0 / scalar);
}

}  // namespace frc
