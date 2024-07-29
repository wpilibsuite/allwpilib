// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/geometry/CoordinateAxis.h"

using namespace frc;

CoordinateAxis::CoordinateAxis(double x, double y, double z) : m_axis{x, y, z} {
  m_axis /= m_axis.norm();
}

const CoordinateAxis& CoordinateAxis::N() {
  static const CoordinateAxis instance{1.0, 0.0, 0.0};
  return instance;
}

const CoordinateAxis& CoordinateAxis::S() {
  static const CoordinateAxis instance{-1.0, 0.0, 0.0};
  return instance;
}

const CoordinateAxis& CoordinateAxis::E() {
  static const CoordinateAxis instance{0.0, -1.0, 0.0};
  return instance;
}

const CoordinateAxis& CoordinateAxis::W() {
  static const CoordinateAxis instance{0.0, 1.0, 0.0};
  return instance;
}

const CoordinateAxis& CoordinateAxis::U() {
  static const CoordinateAxis instance{0.0, 0.0, 1.0};
  return instance;
}

const CoordinateAxis& CoordinateAxis::D() {
  static const CoordinateAxis instance{0.0, 0.0, -1.0};
  return instance;
}
