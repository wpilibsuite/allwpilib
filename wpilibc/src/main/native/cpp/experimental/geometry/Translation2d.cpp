/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/experimental/geometry/Translation2d.h"
#include <cmath>

using namespace frc::experimental;

Translation2d::Translation2d() : m_X(0.0), m_Y(0.0) {}

Translation2d::Translation2d(const double x, const double y) : m_X(x), m_Y(y) {}

double Translation2d::Distance(const Translation2d& other) const {
  return std::hypot(other.m_X - m_X, other.m_Y - m_Y);
}

double Translation2d::Norm() const { return std::hypot(m_X, m_Y); }

Translation2d Translation2d::RotateBy(const Rotation2d& other) const {
  return {m_X * other.Cos() - m_Y * other.Sin(),
          m_X * other.Sin() + m_Y * other.Cos()};
}

Translation2d Translation2d::operator+(const Translation2d& other) const {
  return {X() + other.X(), Y() + other.Y()};
}

void Translation2d::operator+=(const Translation2d& other) {
  m_X += other.m_X;
  m_Y += other.m_Y;
}

Translation2d Translation2d::operator-(const Translation2d& other) const {
  return *this + -other;
}

void Translation2d::operator-=(const Translation2d& other) {
  *this += -other;
}

Translation2d Translation2d::operator-() const { return {-m_X, -m_Y}; }

Translation2d Translation2d::operator*(const double scalar) const {
  return {scalar * m_X, scalar * m_Y};
}

void Translation2d::operator*=(const double scalar) {
  m_X *= scalar;
  m_Y *= scalar;
}

Translation2d Translation2d::operator/(const double scalar) const {
  return *this * (1.0 / scalar);
}

void Translation2d::operator/=(const double scalar) {
  *this *= (1.0 / scalar);
}
