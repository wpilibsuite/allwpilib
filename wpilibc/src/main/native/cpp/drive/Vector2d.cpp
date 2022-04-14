// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/drive/Vector2d.h"

#include <cmath>

#include <wpi/numbers>

using namespace frc;

Vector2d::Vector2d(double x, double y) {
  this->x = x;
  this->y = y;
}

void Vector2d::Rotate(double angle) {
  double cosA = std::cos(angle * (wpi::numbers::pi / 180.0));
  double sinA = std::sin(angle * (wpi::numbers::pi / 180.0));
  double out[2];
  out[0] = x * cosA - y * sinA;
  out[1] = x * sinA + y * cosA;
  x = out[0];
  y = out[1];
}

double Vector2d::Dot(const Vector2d& vec) const {
  return x * vec.x + y * vec.y;
}

double Vector2d::Magnitude() const {
  return std::sqrt(x * x + y * y);
}

double Vector2d::ScalarProject(const Vector2d& vec) const {
  return Dot(vec) / vec.Magnitude();
}
