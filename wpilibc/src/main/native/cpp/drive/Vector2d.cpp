/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/drive/Vector2d.h"

#include <cmath>

using namespace frc;

constexpr double kPi = 3.14159265358979323846;

Vector2d::Vector2d(double x, double y) {
  this->x = x;
  this->y = y;
}

void Vector2d::Rotate(double angle) {
  double cosA = std::cos(angle * (kPi / 180.0));
  double sinA = std::sin(angle * (kPi / 180.0));
  double out[2];
  out[0] = x * cosA - y * sinA;
  out[1] = x * sinA + y * cosA;
  x = out[0];
  y = out[1];
}

double Vector2d::Dot(const Vector2d& vec) const {
  return x * vec.x + y * vec.y;
}

double Vector2d::Magnitude() const { return std::sqrt(x * x + y * y); }

double Vector2d::ScalarProject(const Vector2d& vec) const {
  return Dot(vec) / vec.Magnitude();
}
