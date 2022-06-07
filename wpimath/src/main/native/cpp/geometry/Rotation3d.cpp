// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/Rotation3d.h"

#include <cmath>

#include <wpi/numbers>

#include "units/math.h"

using namespace frc;

Rotation3d::Rotation3d(const Quaternion& q) {
  m_q = q.Normalize();
}

Rotation3d::Rotation3d(units::radian_t roll, units::radian_t pitch,
                       units::radian_t yaw) {
  // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Euler_angles_to_quaternion_conversion
  double cr = units::math::cos(roll * 0.5);
  double sr = units::math::sin(roll * 0.5);

  double cp = units::math::cos(pitch * 0.5);
  double sp = units::math::sin(pitch * 0.5);

  double cy = units::math::cos(yaw * 0.5);
  double sy = units::math::sin(yaw * 0.5);

  m_q = Quaternion{cr * cp * cy + sr * sp * sy, sr * cp * cy - cr * sp * sy,
                   cr * sp * cy + sr * cp * sy, cr * cp * sy - sr * sp * cy};
}

Rotation3d::Rotation3d(const Vectord<3>& axis, units::radian_t angle) {
  double norm = axis.norm();
  if (norm == 0.0) {
    return;
  }

  // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Definition
  Vectord<3> v = axis / norm * units::math::sin(angle / 2.0);
  m_q = Quaternion{units::math::cos(angle / 2.0), v(0), v(1), v(2)};
}

Rotation3d Rotation3d::operator+(const Rotation3d& other) const {
  return RotateBy(other);
}

Rotation3d Rotation3d::operator-(const Rotation3d& other) const {
  return *this + -other;
}

Rotation3d Rotation3d::operator-() const {
  return Rotation3d{m_q.Inverse()};
}

Rotation3d Rotation3d::operator*(double scalar) const {
  // https://en.wikipedia.org/wiki/Slerp#Quaternion_Slerp
  if (m_q.W() >= 0.0) {
    return Rotation3d{{m_q.X(), m_q.Y(), m_q.Z()},
                      2.0 * units::radian_t{scalar * std::acos(m_q.W())}};
  } else {
    return Rotation3d{{-m_q.X(), -m_q.Y(), -m_q.Z()},
                      2.0 * units::radian_t{scalar * std::acos(-m_q.W())}};
  }
}

bool Rotation3d::operator==(const Rotation3d& other) const {
  return m_q == other.m_q;
}

bool Rotation3d::operator!=(const Rotation3d& other) const {
  return !operator==(other);
}

Rotation3d Rotation3d::RotateBy(const Rotation3d& other) const {
  return Rotation3d{other.m_q * m_q};
}

const Quaternion& Rotation3d::GetQuaternion() const {
  return m_q;
}

units::radian_t Rotation3d::X() const {
  double w = m_q.W();
  double x = m_q.X();
  double y = m_q.Y();
  double z = m_q.Z();

  // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
  return units::radian_t{
      std::atan2(2.0 * (w * x + y * z), 1.0 - 2.0 * (x * x + y * y))};
}

units::radian_t Rotation3d::Y() const {
  double w = m_q.W();
  double x = m_q.X();
  double y = m_q.Y();
  double z = m_q.Z();

  // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
  double ratio = 2.0 * (w * y - z * x);
  if (std::abs(ratio) >= 1.0) {
    return units::radian_t{std::copysign(wpi::numbers::pi / 2.0, ratio)};
  } else {
    return units::radian_t{std::asin(ratio)};
  }
}

units::radian_t Rotation3d::Z() const {
  double w = m_q.W();
  double x = m_q.X();
  double y = m_q.Y();
  double z = m_q.Z();

  // https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles#Quaternion_to_Euler_angles_conversion
  return units::radian_t{
      std::atan2(2.0 * (w * z + x * y), 1.0 - 2.0 * (y * y + z * z))};
}

Vectord<3> Rotation3d::Axis() const {
  double norm = std::hypot(m_q.X(), m_q.Y(), m_q.Z());
  if (norm == 0.0) {
    return {0.0, 0.0, 0.0};
  } else {
    return {m_q.X() / norm, m_q.Y() / norm, m_q.Z() / norm};
  }
}

units::radian_t Rotation3d::Angle() const {
  double norm = std::hypot(m_q.X(), m_q.Y(), m_q.Z());
  return units::radian_t{2.0 * std::atan2(norm, m_q.W())};
}

Rotation2d Rotation3d::ToRotation2d() const {
  return Rotation2d{Z()};
}
