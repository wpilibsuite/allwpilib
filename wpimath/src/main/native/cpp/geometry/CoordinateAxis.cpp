// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/CoordinateAxis.h"

using namespace frc;

CoordinateAxis::CoordinateAxis(double x, double y, double z) : m_axis{x, y, z} {
  m_axis /= m_axis.norm();
}

CoordinateAxis CoordinateAxis::N() {
  static CoordinateAxis instance{1.0, 0.0, 0.0};
  return instance;
}

CoordinateAxis CoordinateAxis::S() {
  static CoordinateAxis instance{-1.0, 0.0, 0.0};
  return instance;
}

CoordinateAxis CoordinateAxis::E() {
  static CoordinateAxis instance{0.0, -1.0, 0.0};
  return instance;
}

CoordinateAxis CoordinateAxis::W() {
  static CoordinateAxis instance{0.0, 1.0, 0.0};
  return instance;
}

CoordinateAxis CoordinateAxis::U() {
  static CoordinateAxis instance{0.0, 0.0, 1.0};
  return instance;
}

CoordinateAxis CoordinateAxis::D() {
  static CoordinateAxis instance{0.0, 0.0, -1.0};
  return instance;
}
